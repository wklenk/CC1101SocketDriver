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

#include "DataFrame.hpp"

#include "AddressSpace.hpp"

static const int RSSI_OFFSET = 74;

DataFrame::DataFrame(Spi* spi) {
	this->spi = spi;

	this->status = 0x00;

	this->len = 0;
	this->srcAddress = 0;
	this->destAddress = 0;

	this->rssi = 0;
	this->lqi = 0;
}

/**
 * This method must only be called if we can be sure that actually a
 * complete frame exists in the RX FIFO. This method has no extra checking
 * for incomplete frames.
 */
int DataFrame::receive() {

	// Read Byte 0: Number of following bytes (n).
	this->status = this->spi->readSingleByte(ADDR_RXTX_FIFO, this->len);

	// Read Byte 1: Destination address
	this->spi->readSingleByte(ADDR_RXTX_FIFO, this->destAddress);

	// Read Byte 2: Source address
	this->spi->readSingleByte(ADDR_RXTX_FIFO, this->srcAddress);

	// Read Byte 3 to Byte n: Payload
	this->len -= 2; // Subtract the 2 address bytes
	this->spi->readBurst(ADDR_RXTX_FIFO, this->buffer, this->len);

	// Read Byte n+1: RSSI (Received Signal Strength Indicator)
	uint8_t rssiEncoded;
	this->spi->readSingleByte(ADDR_RXTX_FIFO, rssiEncoded);

	// RSSI is coded as 2's complement see section 17.3 RSSI of the CC101 datasheet
	if (rssiEncoded >= 128) {
		this->rssi = ((rssiEncoded - 256) >> 1) - RSSI_OFFSET;
	} else {
		this->rssi = (rssiEncoded >> 1) - RSSI_OFFSET;
	}

	// Read Byte n+2: LQI (Link Quality Indicator)
	this->spi->readSingleByte(ADDR_RXTX_FIFO, this->lqi);
	// Checksum OK?
	if ((this->lqi & 0x80) == 0) {
		return -1; // CRC error in received data
	}

	this->lqi = this->lqi & 0x7F; // Strip off the CRC bit

	// Handle potential RX overflows by flushing the RF FIFO
	if ((this->status & 0xF0) == 0x60) {
		this->spi->readStrobe(STROBE_SFRX);
	}

	return 0;
}


