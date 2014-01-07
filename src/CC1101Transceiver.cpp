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

#include "CC1101Transceiver.hpp"

const int FIFO_LENGTH = 64;
uint8_t fifo[FIFO_LENGTH];

CC1101Transceiver::CC1101Transceiver(Spi* spi, Gpio* gpio) {
	this->spi = spi;
	this->gpio = gpio;
}

/**
 * Receive a data frame.
 * We use the "variable packet length mode", so the first byte of the received
 * data specifies the payload length. The payload length is limited to 255 bytes.
 *
 * The caller is responsible to provide a buffer that provides space for at
 * least 255 bytes.
 *
 * The code is kind of time critical: Don't add statements that cause
 * additional I/O operations (e.g. debug printf statements). Doing so may
 * lead to RX FIFO overflow.
 */
int CC1101Transceiver::receive(uint8_t buffer[], size_t& nbytes) {

	// When this method is entered, when RX FIFO is filled at or above
	// the RX FIFO threshold or the end of packet is reached.
	// See CC1101 configuration register IOCFG2 = 0x01

	uint8_t rxBytes;
	this->spi->readBurst(ADDR_RX_BYTES, &rxBytes, 1);

	if ((rxBytes & 0x80) > 0) {
		DateTime::print();
		printf("RX FIFO Overflow (rxbytes=0x%.2X)\n", rxBytes);
		return -1;
	}

	rxBytes &= 0x7f; // Clear RX FIFO Overflow bit

	assert(rxBytes > 0); // There should be some bytes to read
	assert(rxBytes <= FIFO_LENGTH);
	this->spi->readBurst(ADDR_RXTX_FIFO, fifo, rxBytes);

	// CC1101 variable length mode: First byte after sync word holds the length
	uint8_t variableLength = fifo[0];

	// CC1101 receiver is configured to add RSSI byte and LQI byte at the end
	// of the RX FIFO. This adds 2 more bytes to the RX FIFO.
	variableLength += 2;

	uint8_t currentLength = 0;
	assert(rxBytes > 1);
	memcpy(buffer + currentLength, fifo + 1, rxBytes - 1);
	currentLength += (rxBytes - 1);

	while (currentLength < variableLength) {

		// Poll if new data has arrived in the RX FIFO
		int cntEmptyLoops = 10;
		do {
			this->spi->readBurst(ADDR_RX_BYTES, &rxBytes, 1);

			if ((rxBytes & 0x80) > 0) {
				DateTime::print();
				printf("RX FIFO Overflow (rxbytes=0x%.2X)\n", rxBytes);
				return -1;
			}

			rxBytes &= 0x7f; // Clear RX FIFO Overflow bit

			if (rxBytes == 0) {
				cntEmptyLoops--;

				if (cntEmptyLoops < 0) {
					DateTime::print();
					printf("RX FIFO Receive Timeout (currentLength=%d variableLength=%d)\n", currentLength, variableLength);
					return -1;
				}
			}

		} while (rxBytes == 0);

		assert(rxBytes > 0); // There should be some bytes to read
		assert(rxBytes <= FIFO_LENGTH);
		this->spi->readBurst(ADDR_RXTX_FIFO, fifo, rxBytes);

		memcpy(buffer + currentLength, fifo, rxBytes);
		currentLength += rxBytes;
	}

	nbytes = currentLength;

	DateTime::print();
	printf("Received message (variableLength=%d currentLength=%d)\n", variableLength, currentLength);

	return 0;
}

/**
 * Transmit the frame.
 * We use the "variable packet length mode", so the first byte specifies the
 * payload length. The payload length is limited to 255 bytes.
 */
int CC1101Transceiver::transmit(const uint8_t buffer[], size_t nbytes) {

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

