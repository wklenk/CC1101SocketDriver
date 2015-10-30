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

#ifndef SERIAL_BITSTREAM_H_
#define SERIAL_BITSTREAM_H_

#define DEBUG_SHOW_HEX
//#define DEBUG_SHOW_BINARY

#include <stdint.h>

/**
 * Assumes that the output of a serial communication stream/line, including start- and stop-bits
 * is send over the air. The order of the bits in the bytes has also to be reversed as the data
 * is mapped to a buffer of bytes.
 */
class SerialBitstream {

public:
	SerialBitstream();

	/**
	 * From a serial bitstream, removes the start- and stop-bits from the bit stream and reverses
	 * the bit order. The result is a buffer of bytes.
	 * The caller is responsible to provide a buffer for the resulting decoded bytes
	 * that is big enough (at least 4/5 as big as the input buffer)
	 *
	 * @param inputBuffer Buffer of undecoded bytes.
	 * @param inputBufferLen Length of the buffer of undecoded bytes.
	 * @param outputBuffer Resulting buffer of decoded bytes.
	 * @param outputBufferLen Length of the resulting buffer of decoded bytes.
	 */
	void decode(uint8_t *inputBuffer, size_t inputBufferLen, uint8_t *outputBuffer, size_t& outputBufferLen);

	/**
	 * From a buffer of bytes, reverses the bit order of the input buffer and adds a start bit
	 * before and a stop bit after each byte. The result is a serial bitstream.
	 * The caller is responsible to provide a buffer for the resulting encoded bytes
	 * that is big enough (at least 5/4 as big as the input buffer).
	 *
	 * @param inputBuffer Buffer of unencoded bytes.
	 * @param inputBufferLen Length of the buffer of unencoded bytes.
	 * @param outputBuffer Resulting buffer of encoded bytes.
	 * @param outputBufferLen Length of resulting buffer of encoded bytes.
	 */
	void encode(uint8_t *inputBuffer, size_t inputBufferLen, uint8_t *outputBuffer, size_t& outputBufferLen);

	/**
	 * Prints the specified buffer.
	 *
	 * @param buffer Buffer of bytes to print out.
	 * @param len Length of this buffer.
	 */
	void show(uint8_t* buffer, size_t len);
};

#endif /* SERIAL_BITSTREAM_H_ */
