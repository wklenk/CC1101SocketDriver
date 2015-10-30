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
#include "SerialBitstream.hpp"
#include "Manchester.hpp"
#include "RadiatorControllerDataFrame.hpp"

// Reverse the bit order and add/remote start/stop bits
SerialBitstream serialBitstream;

// Manchester En/Decoder
Manchester manchester;

/**
 * Decodes the RSSI value
 */
static int RSSIDecode(uint8_t rssiEnc){
	int rssi;

	if (rssiEnc >= 128) {
		rssi = (rssiEnc - 256) >> 1;
	} else {
		rssi = rssiEnc >> 1;
	}

	return rssi - 74;
}

RadiatorControllerDataFrame::RadiatorControllerDataFrame(Protocol* protocol) : IDataFrame(protocol) {
	this->len = 0;
	this->rssi = 0;
	this->lqi = 0;
}

/**
 * Returns 0 if a valid data frame could be read from the RX FIFO
 * and decoded successfully.
 * Returns -1 on error.
 */
int RadiatorControllerDataFrame::receive() {

	// 2 extra bytes for RSSI and LQI at the end of the message
	uint8_t bytesFromFifo[FIFO_LENGTH+2];
	uint8_t bytesInCorrectOrder[FIFO_LENGTH+2];
	uint8_t bytesAfterManchesterDecoding[(FIFO_LENGTH+2)/2];

	size_t nbytes;
	size_t nbytesInCorrectOrder;
	size_t nbytesAfterManchesterDecoding;

	int rc = this->protocol->receive(bytesFromFifo, nbytes);
	assert(nbytes <= (FIFO_LENGTH+2));
	if (rc >= 0) {
		this->rssi = bytesFromFifo[nbytes-2];
		this->lqi = bytesFromFifo[nbytes-1];
		nbytes -= 2; // Remove RSSI and LQI from buffer

		// Reverse the bit order and remote the start/stop bits
		serialBitstream.decode(bytesFromFifo, nbytes, bytesInCorrectOrder, nbytesInCorrectOrder);

		// Each HR80 message starts with this preamble: 0x33 0x55 0x53
		if (bytesInCorrectOrder[0] == 0x33 && bytesInCorrectOrder[1] == 0x55 && bytesInCorrectOrder[2] == 0x53) {

			// We know that there is some kind of RF postamble 0x35
			// ... that marks the end of the message.
			// Find out the position of this message end marker.
			int posMessageEndMarker = -1;
			for (int i=3 ; i<nbytesInCorrectOrder ; i++) {

				if (bytesInCorrectOrder[i] == 0x35) {
					posMessageEndMarker = i;
					break;
				}
			}

			if (posMessageEndMarker > 0) {
				int rcManchester = manchester.decode(
						bytesInCorrectOrder + 3,
						posMessageEndMarker - 3,
						bytesAfterManchesterDecoding,
						nbytesAfterManchesterDecoding);

				if (rcManchester > 0) {
					memcpy(this->buffer, bytesAfterManchesterDecoding, nbytesAfterManchesterDecoding);
					this->len = nbytesAfterManchesterDecoding;

					DateTime::print();
					printf("RadiatorControllerDataFrame received (length=%d RSSI=%ddBm LQI=0x%.2X)\n",
							nbytesAfterManchesterDecoding,RSSIDecode(this->rssi), this->lqi);
				} else {
					// Could not Manchester decode for some reason.
					// May happen due to transmission errors.
					return -1;
				}
				return 0;

			} else {
				printf("ERROR: Cannot find message end marker.\n");
			}

		} else {
			printf("Unknown message header: 0x%.2X 0x%.2X 0x%.2X\n",
					bytesInCorrectOrder[0],
					bytesInCorrectOrder[1],
					bytesInCorrectOrder[2]);
		}
	}

	return -1;
}

/**
 * Writes the frame into the TX FIFO and makes the CC1101 transmit
 * the data by sending a STX strobe command.
 */
int RadiatorControllerDataFrame::transmit() {

	return 0;
}

/**
 * Writes the contents of this data frame to a file descriptor.
 */
void RadiatorControllerDataFrame::writeToSocket(int fd) {

	assert(fd >= 0);

	const size_t MAX_LINE_LENGTH = 768; // Not exact, but should be enough
	char line[MAX_LINE_LENGTH];
	char tmp[16];

	line[0] = '\0';

	//  payload (HEX) NL
	for (int i = 0; i<this->len; i++) {
		snprintf(tmp, 16, "%.2X", this->buffer[i]);
		strcat(line, tmp);
	}

	// Add RSSI (as HEX)
	snprintf(tmp, 16, "-");
	strcat(line, tmp);
	snprintf(tmp, 16, "%.2X", this->rssi);
	strcat(line, tmp);

	snprintf(tmp, 16, "\n");
	strcat(line, tmp);

	write(fd, line, strlen(line));

	assert(strlen(line) < MAX_LINE_LENGTH);
}


