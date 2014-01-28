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

#include "RawDataFrame.hpp"

RawDataFrame::RawDataFrame(Protocol* protocol) : IDataFrame(protocol) {
	this->len = 0;
}

/**
 * Returns 0 if a data frame could be read from the RX FIFO.
 * Returns -1 on error.
 */
int RawDataFrame::receive() {

	uint8_t tmp[MAX_PAYLOAD_BYTES + 4];
	size_t nbytes;

	int rc = this->protocol->receive(tmp, nbytes);
	if (rc >= 0) {
		size_t payloadLength = nbytes - 2; // - 2 for RSSI, LQI
		memcpy(this->buffer, tmp, payloadLength);
		this->len = payloadLength;

		DateTime::print();
		printf("RawDataFrame received (length=%d)\n", nbytes);
	}

	return 0;
}

/**
 * Writes the frame into the TX FIFO and makes the CC1101 transmit
 * the data by sending a STX strobe command.
 */
int RawDataFrame::transmit() {

	return 0;
}

/**
 * Writes the contents of this data frame to a file descriptor.
 */
void RawDataFrame::writeToSocket(int fd) {

	assert(fd >= 0);

	const size_t MAX_LINE_LENGTH = 768; // Not exact, but should be enough
	char line[MAX_LINE_LENGTH];
	char tmp[16];

	line[0] = '\0';

	//  payload (HEX) NL
	for (size_t i = 0; i<this->len; i++) {
		snprintf(tmp, 16, "%.2X", this->buffer[i]);
		strcat(line, tmp);
	}
	snprintf(tmp, 16, "\n");
	strcat(line, tmp);

	write(fd, line, strlen(line));

	assert(strlen(line) < MAX_LINE_LENGTH);
}


