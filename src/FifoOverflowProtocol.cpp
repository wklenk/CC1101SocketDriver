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

#include "FifoOverflowProtocol.hpp"

FifoOverflowProtocol::FifoOverflowProtocol(Spi* spi) {
	this->spi = spi;
}

/**
 * Just read out the complete RX FIFO of the CC1101.
 *
 *
 * Note: RSSI and LQI are appended to the end of the message.
 *
 * The caller is responsible to provide a buffer that provides space for at
 * least 64 + 2 bytes.
 */
int FifoOverflowProtocol::receive(uint8_t buffer[], size_t& nbytes) {

	// When this method is entered, then RX FIFO has overflowed.
	// See CC1101 configuration register IOCFG2 = 0x04.

	// Read the complete RX FIFO buffer
	this->spi->readBurst(ADDR_RXTX_FIFO, fifo, FIFO_LENGTH);

	memcpy(buffer, fifo, FIFO_LENGTH);
	nbytes = FIFO_LENGTH;

	// Read RSSI
	uint8_t buffer_rssi[1];
	this->spi->readBurst(ADDR_RSSI, buffer_rssi, 1);
	buffer[nbytes++] = buffer_rssi[0];

	// Read LQI
	uint8_t buffer_lqi[1];
	this->spi->readBurst(ADDR_LQI, buffer_lqi, 1);
	buffer[nbytes++] = buffer_lqi[0];

	return 0;
}

/**
 *
 */
int FifoOverflowProtocol::transmit(const uint8_t buffer[], size_t nbytes) {

	return 0;
}

