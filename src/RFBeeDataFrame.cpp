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

#include "RFBeeDataFrame.hpp"


static const int DEFAULT_OUTPUT_FORMAT = 4; // I prefer hex output

/**
 * Decodes the RSSI value added by the CC1101 receiver.
 */
static int RSSIDecode(uint8_t rssiEnc){
	int rssi;

	if (rssiEnc >= 128) {
		rssi = (rssiEnc - 256) >> 1;
	} else {
		rssi = rssiEnc >> 1;
	}

	return rssi - 74;

  return rssi;
}

RFBeeDataFrame::RFBeeDataFrame(Protocol* protocol) : IDataFrame(protocol) {

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
int RFBeeDataFrame::receive() {

	uint8_t tmp[MAX_PAYLOAD_BYTES + 4];
	size_t nbytes;

	int rc = this->protocol->receive(tmp, nbytes);

	if (rc >= 0) {
		size_t cnt = 0;
		this->destAddress = tmp[cnt++];
		this->srcAddress = tmp[cnt++];

		size_t payloadLength = nbytes - 4; // dstAddress, srcAddress, RSSI, LQI
		memcpy(this->buffer, tmp + cnt, payloadLength);
		cnt += payloadLength;
		this->len = payloadLength;

		this->rssi = tmp[cnt++];
		this->lqi = tmp[cnt++];

		DateTime::print();
		printf("RFBeeDataFrame received (length=%d destAddress=0x%.2X srcAddress=0x%.2X RSSI=%ddBm LQI=0x%.2X)\n",
				nbytes, this->destAddress, this->srcAddress, RSSIDecode(this->rssi), this->lqi);

		assert(nbytes == cnt);

		// Checksum OK?
		if ((this->lqi & 0x80) == 0) {
			DateTime::print();
			printf("Receiver detected CRC error.\n");
			return -1; // CRC error in received data
		}

		this->lqi = this->lqi & 0x7F; // Strip off the CRC bit
	}

	return 0;
}

/**
 * Writes the frame into the TX FIFO and makes the CC1101 transmit
 * the data by sending a STX strobe command.
 */
int RFBeeDataFrame::transmit() {

	return 0;
}

/**
 * Writes the contents of this data frame to a file descriptor.
 */
void RFBeeDataFrame::writeToSocket(int fd) {

	assert(fd >= 0);

	const size_t MAX_LINE_LENGTH = 768; // Not exact, but should be enough
	char line[MAX_LINE_LENGTH];
	char tmp[16];

	size_t cnt = 0;

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


