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

#include "AddressSpace.hpp"
#include "DateTime.hpp"

#include "DataFrame.hpp"


static const int DEFAULT_OUTPUT_FORMAT = 4; // I prefer hex output

DataFrame::DataFrame(Spi* spi) {
	this->spi = spi;

	this->len = 0;
	this->srcAddress = 0;
	this->destAddress = 0;

	this->rssi = 0;
	this->lqi = 0;

	this->outputFormat = DEFAULT_OUTPUT_FORMAT;
}

/**
 * This method must only be called if we can be sure that actually a
 * complete frame exists in the RX FIFO.
 * There are some basic checks to make sure the frame in the RXFIFO is
 * valid and not just random garbage. However, this may NOT be sufficient
 * in some cases.
 *
 * Returns 0 if a data frame could be read from the RX FIFO.
 * Returns -1 on error.
 */
int DataFrame::receive() {
	assert(this->spi != NULL);

	uint8_t status;

	// Read Byte 0: Number of following bytes (n).
	status = this->spi->readSingleByte(ADDR_RXTX_FIFO, this->len);

	// Plausibility check: Number of bytes available in the RX FIFO has to be
	// at least greater as number of bytes received in the first byte
	// of the frame
	if ((status & 0x0F) != 0x0F) {
		if(! (status & 0x0F) >= this->len) {
			return -1;
		}
	}

	// Read Byte 1: Destination address
	status = this->spi->readSingleByte(ADDR_RXTX_FIFO, this->destAddress);
	if(! (status & 0x0F) > 0) {
		return -1;
	}

	// Read Byte 2: Source address
	status = this->spi->readSingleByte(ADDR_RXTX_FIFO, this->srcAddress);
	if(! (status & 0x0F) > 0) {
		return -1;
	}

	// Read Byte 3 to Byte n: Payload
	this->len -= 2; // Subtract the 2 address bytes

	// Plausibility check: At least this->len bytes have to be available
	// in the RX FIFO at this point in time
	if ((status & 0x0F) != 0x0F) {
		if(! (status & 0x0F) >= this->len) {
			return -1;
		}
	}

	assert(MAX_PAYLOAD_BYTES >= this->len);
	status = this->spi->readBurst(ADDR_RXTX_FIFO, this->buffer, this->len);
	if(! (status & 0x0F) > 0) {
		return -1;
	}

	// Read Byte n+1: RSSI (Received Signal Strength Indicator)
	status = this->spi->readSingleByte(ADDR_RXTX_FIFO, this->rssi);
	if(! (status & 0x0F) > 0) {
		return -1;
	}

	// Read Byte n+2: LQI (Link Quality Indicator)
	this->spi->readSingleByte(ADDR_RXTX_FIFO, this->lqi);
	// Checksum OK?
	if ((this->lqi & 0x80) == 0) {
		return -1; // CRC error in received data
	}

	this->lqi = this->lqi & 0x7F; // Strip off the CRC bit

	return 0;
}

/**
 * Writes the contents of this data frame to a file descriptor.
 */
void DataFrame::writeToSocket(int fd) {

	assert(fd >= 0);

	const int MAX_LINE_LENGTH = 768; // Not exact, but should be enough
	char line[MAX_LINE_LENGTH];
	char tmp[16];

	int cnt = 0;

	switch(this->outputFormat) {

	case 0 :
		// 0: Payload only
		write(fd, (void*) this->buffer, (size_t) this->len);
		break;

	case 1 :
		// 1: source, dest, payload
		line[cnt++] = this->srcAddress;
		line[cnt++] = this->destAddress;
		memcpy(line + cnt, this->buffer, this->len); cnt += this->len;
		write(fd, line, cnt);

		assert (cnt < MAX_LINE_LENGTH);
		break;

	case 2 :
		// 2: payload len, source, dest, payload, rssi, lqi
		line[cnt++] = this->len;
		line[cnt++] = this->srcAddress;
		line[cnt++] = this->destAddress;
		memcpy(line + cnt, this->buffer, this->len); cnt += this->len;
		line[cnt++] = this->rssi;
		line[cnt++] = this->lqi;
		write(fd, line, cnt);

		assert (cnt < MAX_LINE_LENGTH);
		break;

	case 3 :
		//  3: payload len (DEC), source (DEC), dest (DEC), payload, rssi (DEC), lqi (DEC) NL
		snprintf(line, 128, "%d,%d,%d,", this->len, this->srcAddress, this->destAddress);
		cnt = strlen(line);
		memcpy(line + cnt, this->buffer, this->len); cnt += this->len;
		line[cnt++] = '\0';
		snprintf(tmp, 16, ",%d,%d\n", this->rssi, this->lqi);
		strcat(line, tmp);
		write(fd, line, strlen(line));

		assert(strlen(line) < MAX_LINE_LENGTH);
		break;

	case 4:
		//  payload len (HEX) source (HEX) dest (HEX) payload (HEX) rssi (HEX) lqi (HEX) NL
		snprintf(line, 128, "%.2X %.2X %.2X ", this->len, this->srcAddress, this->destAddress);
		for (int i = 0; i<this->len; i++) {
			snprintf(tmp, 16, "%.2X", this->buffer[i]);
			strcat(line, tmp);
		}
		snprintf(tmp, 16, " %.2X %.2X\n", this->rssi, this->lqi);
		strcat(line, tmp);

		write(fd, line, strlen(line));

		assert(strlen(line) < MAX_LINE_LENGTH);
		break;
	}
}
