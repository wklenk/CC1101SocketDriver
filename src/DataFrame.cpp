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

DataFrame::DataFrame(Spi* spi) {
	this->spi = spi;
}

/**
 * This method must only be called if we can be sure that actually a
 * complete frame exists in the RX FIFO. This method has no extra checking
 * for incomplete frames.
 */
int DataFrame::receive(uint8_t buffer[], size_t& nbytes, uint8_t& srcAddress, uint8_t& destAddress, uint8_t& rssi , uint8_t&  lqi) {

	// Read Byte 0: Number of following bytes (n).
	uint8_t len;
	uint8_t status = this->spi->readSingleByte(ADDR_RXTX_FIFO, len);

	// Read Byte 1: Destination address
	this->spi->readSingleByte(ADDR_RXTX_FIFO, destAddress);

	// Read Byte 2: Source address
	this->spi->readSingleByte(ADDR_RXTX_FIFO, srcAddress);

	// Read Byte 3 to Byte n: Payload
	len -= 2; // Subtract the 2 address bytes
	this->spi->readBurst(ADDR_RXTX_FIFO, buffer, len);
	nbytes = len;

	// Read Byte n+1: RSSI (Received Signal Strength Indicator)
	this->spi->readSingleByte(ADDR_RXTX_FIFO, rssi);

	// RSSI is coded as 2's complement see section 17.3 RSSI of the CC101 datasheet
	if (rssi >= 128) {
		rssi = ((rssi - 256) >> 1) - 74;
	} else {
		rssi = (rssi >> 1) - 74;
	}

	// Read Byte n+2: LQI (Link Quality Indicator)
	this->spi->readSingleByte(ADDR_RXTX_FIFO, lqi);
	// Checksum OK?
	if ((lqi & 0x80) == 0) {
		return -1; // CRC error in received data
	}

	lqi = lqi & 0x7F; // Strip off the CRC bit

	// Handle potential RX overflows by flushing the RF FIFO
	if ((status & 0xF0) == 0x60) {
		this->spi->readStrobe(STROBE_SFRX);
	}

	return 0;
}


