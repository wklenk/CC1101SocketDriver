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

#ifndef MANCHESTER_HPP_
#define MANCHESTER_HPP_

#define MAN_DECODING_OK      0
#define MAN_DECODING_ERROR   1

#include <stdint.h>

class Manchester {

public:
	void encodeByte(uint8_t *unencoded, uint8_t *encoded);

	/**
	 * Returns MAN_DECODING_ERROR if decoding fails.
	 */
	uint8_t decodeByte(uint8_t *undecoded, uint8_t *decoded);

	uint8_t encode(uint8_t *inputBuffer, size_t inputBufferLen, uint8_t *outputBuffer, size_t& outputBufferLen);

	/**
	 * Returns 0 if decoding fails.
	 */
	uint8_t decode(uint8_t *inputBuffer, size_t inputBufferLen, uint8_t *outputBuffer, size_t& outputBufferLen);

};

#endif /* MANCHESTER_HPP_ */
