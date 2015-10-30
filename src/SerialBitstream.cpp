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

#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
using namespace std;

#include "SerialBitstream.hpp"


SerialBitstream::SerialBitstream() {
}


void SerialBitstream::decode(uint8_t *inputBuffer, size_t inputBufferLen, uint8_t *outputBuffer, size_t& outputBufferLen) {

	// Remove the start- and stop-bits in the bitstream
	// #0123456 7##01234 567##012 34567##0 1234567# (# -> Start/Stop bit)
	// is decoded to:
	// 76543210 76543210 76543210 76543210

	int src_bytepos;
	int src_bitpos;

	int dest_bytepos;
	int dest_bitpos;

	int j = 0;
	for (int i=0 ; i < (inputBufferLen * 8) ; i++) {

		if (i % 10 == 0) {
			// Start bit. Ignore.
		}
		else if ((i - 9) % 10 == 0) {
			// Stop bit. Ignore.
		}
		else {
			src_bytepos = i / 8;
			src_bitpos = i % 8;

			dest_bytepos = j / 8;
			dest_bitpos = j % 8;

			uint8_t mask = 1 << (7 - src_bitpos);
			if ((inputBuffer[src_bytepos] & mask) > 0) {
				outputBuffer[dest_bytepos] |= 1 << dest_bitpos;
			}
			else {
				outputBuffer[dest_bytepos] &= ~(1 << dest_bitpos);
			}

			j++;
		}
	}

	outputBufferLen = (j / 8) + 1;
}

/**
 * Reverses the bit order of the input data and adds a start bit before and a stop bit
 * after each byte.
 *
 * @param inputBuffer Points to the unencoded data.
 * @param inputBufferLen Number of bytes of unencoded data.
 * @param outputBuffer Points to the encoded data.
 * @param outputBufferLen Number of bytes of encoded data.
 */
void SerialBitstream::encode(uint8_t *inputBuffer, size_t inputBufferLen, uint8_t *outputBuffer, size_t& outputBufferLen) {

	// Adds a start and stop bit and reverses the bit order.
	// 76543210 76543210 76543210 76543210
	// is encoded to:
	// #0123456 7##01234 567##012 34567##0 1234567# (# -> Start/Stop bit)

	//bitstream.show(inputBuffer, inputBufferLen);

	int bytepos;
	int bitpos;

	int j = 0;
	for (int i=0 ; i < (inputBufferLen * 8) ; i++) {

		if (j % 10 == 0) {
			// Insert start bit (0)
			bytepos = j / 8;
			bitpos = j % 8;

			outputBuffer[bytepos] &= ~(1 << (7 - bitpos));
			j++;
		}

		bytepos = i / 8;
		bitpos = i % 8;

		uint8_t mask = 1 << bitpos;
		if ((inputBuffer[bytepos] & mask) > 0) {

			bytepos = j / 8;
			bitpos = 7 - (j % 8);

			outputBuffer[bytepos] |= 1 << bitpos;

		} else {

			bytepos = j / 8;
			bitpos = 7 - (j % 8);

			outputBuffer[bytepos] &= ~(1 << bitpos);
		}

		j++;

		if ((j - 9) % 10 == 0) {
			// Insert stop bit (1)
			bytepos = j / 8;
			bitpos = j % 8;

			outputBuffer[bytepos] |= 1 << (7 - bitpos);
			j++;
		}
	}


	outputBufferLen = (j / 8) + 1;

	//bitstream.show(outputBuffer, *outputBufferLen);

}

void SerialBitstream::show(uint8_t* buffer, size_t len)
{
#ifdef DEBUG_SHOW_HEX
	for (int i=0 ; i<len ; i++) {
		if (!(i % 16))
			puts("");

		printf("%02X ", buffer[i]);
	}
	printf("\n");
#endif

#ifdef DEBUG_SHOW_BINARY
	const uint8_t *ptr;
	for ( ptr = buffer; len--; ptr++ )
	{
		uint8_t mask;
		for ( mask = 0x80 ; mask ; mask >>= 1 )
		{
			(mask & *ptr) > 0 ?  printf("1") : printf("0");
		}
		printf(" ");
	}
	printf("\n");
#endif
}

