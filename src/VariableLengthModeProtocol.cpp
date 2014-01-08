/*
 * rfcc1101 - SPI Protocol Driver for TI CC1101 RF communication module.
 *
 * Copyright (C) 2013 Wolfgang Klenk <wolfgang.klenk@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "AddressSpace.hpp"
#include "DateTime.hpp"

#include "VariableLengthModeProtocol.hpp"

const int FIFO_LENGTH = 64;
uint8_t fifo[FIFO_LENGTH];

uint8_t t_rxbytes[FIFO_LENGTH];

VariableLengthModeProtocol::VariableLengthModeProtocol(Spi* spi, Gpio* gpio) {
	this->spi = spi;
	this->gpio = gpio;
}

/**
 * Receive a data frame.
 * We use the "variable packet length mode", so the first byte of the received
 * data specifies the payload length. The payload length is limited to 255 bytes.
 *
 * The caller is responsible to provide a buffer that provides space for at
 * least 255(+2 bytes for RSSI and LQI) bytes.
 *
 * The code is kind of time critical: Don't add statements that cause
 * additional I/O operations (e.g. debug printf statements). Doing so may
 * lead to RX FIFO overflow.
 */
int VariableLengthModeProtocol::receive(uint8_t buffer[], size_t& nbytes) {

	int cnt = 0;

	// When this method is entered, when RX FIFO is filled at or above
	// the RX FIFO threshold or the end of packet is reached.
	// See CC1101 configuration register IOCFG2 = 0x01

	// Read the variable length byte from the RX FIFO
	uint8_t variableLength;
	uint8_t chipStatus = this->spi->readSingleByte(ADDR_RXTX_FIFO, variableLength);
	if (variableLength == 0) {
		DateTime::print();
		printf("RX FIFO received invalid variable length byte = 0x00.\n");

		this->spi->readStrobe(STROBE_SFRX); // Flush the RX FIFO
		return -1;
	}

	if ((chipStatus & 0x70) == 0x60) {
		DateTime::print();
		printf("RX FIFO Overflow when reading first byte. Flushing RX Buffer.\n");

		this->spi->readStrobe(STROBE_SFRX); // Flush the RX FIFO
		return -1;
	}

	// The CC1101 receiver adds 2 bytes at the end of the message:
	// RSSI and LQI.
	variableLength += 2;

	uint8_t rxBytes;
	uint8_t currentLength = 0;
	do {
		// Check how many bytes we can read from the RX FIFO
		this->spi->readBurst(ADDR_RX_BYTES, &rxBytes, 1);

		t_rxbytes[cnt++] = rxBytes; // Debug

		if ((rxBytes & 0x80) > 0) {
			DateTime::print();
			printf("RX FIFO Overflow. Flushing RX Buffer. (rxbytes=0x%.2X)\n", rxBytes);

			for (int i=0 ; i<cnt ; i++) {
				printf("i=%d rxbytes=0x%.2X (0x%.2X) %d (%d)\n", i, t_rxbytes[i],  t_rxbytes[i] & 0x7F, t_rxbytes[i], t_rxbytes[i] & 0x7F);
			}

			this->spi->readStrobe(STROBE_SFRX); // Flush the RX FIFO
			return -1;
		}

		rxBytes &= 0x7f; // Clear RX FIFO Overflow bit

		assert(rxBytes > 0); // There should be some bytes to read

		// Sometimes, rxBytes may have a "buggy" value.
		if (rxBytes > FIFO_LENGTH) {
			// Just read again.
			continue;
		}

		// In case this is the remaining part of the message, read all
		// bytes from the RX FIFO. If not, keep a byte in the RX FIFO.
		if ((currentLength + rxBytes) >= variableLength) {
			this->spi->readBurst(ADDR_RXTX_FIFO, fifo, rxBytes);
			memcpy(buffer + currentLength, fifo, rxBytes);
			currentLength += rxBytes;
		} else {
			// Intentionally keep a byte in the RX FIFO
			this->spi->readBurst(ADDR_RXTX_FIFO, fifo, rxBytes - 1);
			memcpy(buffer + currentLength, fifo, rxBytes - 1);
			currentLength += (rxBytes - 1);

			usleep(2000); // Allow some time to fill the RX FIFO
		}
	} while (currentLength < variableLength);

	nbytes = currentLength;

	DateTime::print();
	printf("Received message (variableLength=%d currentLength=%d)\n", variableLength, currentLength);

	for (int i=0 ; i<cnt ; i++) {
		printf("i=%d rxbytes=0x%.2X (0x%.2X) %d (%d)\n", i, t_rxbytes[i],  t_rxbytes[i] & 0x7F, t_rxbytes[i], t_rxbytes[i] & 0x7F);
	}

	return 0;
}

/**
 * Transmit the frame.
 * We use the "variable packet length mode", so the first byte specifies the
 * payload length. The payload length is limited to 255 bytes.
 */
int VariableLengthModeProtocol::transmit(const uint8_t buffer[], size_t nbytes) {

	/*
	assert(nbytes > 0);
	assert(nbytes <= 255);

	uint8_t status;

	// Make sure the TX FIFO is empty
	while (true) {
		this->spi->readBurst(ADDR_TX_BYTES, &status, 1);

		if ((status & 0x80) > 0) {
			DateTime::print();
			printf("TX FIFO underflow. Flushing TX FIFO.\n");

			this->spi->readStrobe(STROBE_SFTX);

			sleep(100);
		} else {
			// When we are here, the TX FIFO should be empty.
			// If not, then we have an issue in the code.
			assert(status == 0);
			break;
		}
	}

	// Write the "length" byte
	this->spi->writeSingleByte(ADDR_RXTX_FIFO, nbytes);

	// Write the first 63 bytes
	int currentPos = 0;
	size_t currentBytes = 63;
	if (nbytes < currentBytes) {
		currentBytes = nbytes;
	}

	DateTime::print();
	printf("currentPos=%d currentBytes=%d\n", currentPos, currentBytes);

	this->spi->writeBurst(ADDR_RXTX_FIFO, buffer + currentPos, currentBytes);
	currentPos += currentBytes;

	// Start transmission
	this->spi->readStrobe(STROBE_STX);

	// When the STX or SFSTXON command strobe is given while CC1101 is in the
	// RX state, the TX or FSTXON state is only entered if the clear channel
	// requirements are fulfilled. Otherwise, the chip will remain in RX.
	// If the channel the becomes available, the radio will not enter TX
	// or FSTXON state before a new strobe command is sent on the SPI interface.
	// This feature is called TX-if-CCA.

	// For this reason, we actually need to check if transmission actually
	// started
	while (true) {
		status = this->spi->readStrobe(STROBE_SNOP);
		if((status & 0x70) == 0x20) {
			break;
		}

		DateTime::print();
		printf("Current state: 0x%.2X - Waiting for TX.\n", status);
		sleep(100);
	}


	while (true) {
		this->spi->readBurst(ADDR_TX_BYTES, &status, 1);

		assert ((status & 0x80) == 0); // Check for TX FIFO Underflow

		// Some space left in TX FIFO?
		if (status < 64) {
			currentBytes = 64 - status;

			if ((nbytes - currentPos) < currentBytes) {
				currentBytes = nbytes - currentPos;
			}

			if (currentBytes == 0) {
				break;
			}

			DateTime::print();
			printf("status=%d currentPos=%d currentBytes=%d\n", status, currentPos, currentBytes);

			this->spi->writeBurst(ADDR_RXTX_FIFO, buffer + currentPos, currentBytes);
			currentPos += currentBytes;
		}
	}

	*/

	return 0;
}

