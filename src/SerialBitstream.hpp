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

class SerialBitstream {

public:
	SerialBitstream();

	void decode(uint8_t *inputBuffer, size_t inputBufferLen, uint8_t *outputBuffer, size_t& outputBufferLen);
	void encode(uint8_t *inputBuffer, size_t inputBufferLen, uint8_t *outputBuffer, size_t& outputBufferLen);

	void show(uint8_t* buffer, size_t len);

};

#endif /* SERIAL_BITSTREAM_H_ */
