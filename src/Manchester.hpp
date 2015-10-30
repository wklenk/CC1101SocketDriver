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

/**
 * Encodes or decodes a buffer of bytes following the rules of the Manchester Code.
 */
class Manchester {

public:
	/**
	 * Encodes a single byte.
	 * The caller is responsible to provide a buffer for the manchester-encoded bytes
	 * of at least 2 bytes.
	 *
	 * @param unencoded Unencoded byte.
	 * @param encoded Resulting manchester-encoded byte.
	 */
	void encodeByte(uint8_t *unencoded, uint8_t *encoded);

	/**
	 * Decodes a single byte.
	 * The caller is responsible to provide a buffer for the manchester-decoded byte
	 * of at least 1 byte.
	 *
	 * @param undecoded Undecoded byte.
	 * @param decoded Resulting manchester-decoded byte.
	 *
	 * @return MAN_DECODING_ERROR if decoding fails or MAN_DECODING_OK, if
	 * 		   decoding was successful.
	 */
	uint8_t decodeByte(uint8_t *undecoded, uint8_t *decoded);

	/**
	 * Encodes a buffer of bytes.
	 * The caller is responsible to provide a buffer for the resulting manchester-encoded bytes of
	 * at least twice the size of the buffer of the unencoded bytes.
	 *
	 * @param inputBuffer Buffer of unencoded bytes.
	 * @param inputBufferLen Length of buffer of unencoded bytes.
	 * @param outputBuffer Resulting buffer of manchester-encoded bytes.
	 * @param outputBufferLen Length of resulting buffer of manchester-encoded bytes.
	 *
	 * @return Length of resulting buffer of manchester-encoded bytes.
	 */
	uint8_t encode(uint8_t *inputBuffer, size_t inputBufferLen, uint8_t *outputBuffer, size_t& outputBufferLen);

	/**
	 * Decodes a buffer of bytes.
	 * The caller is responsible to provide a buffer for the resulting manchester-decoded bytes of
	 * at least half the size of the buffer of the undecoded bytes.
	 *
	 * @param inputBuffer Buffer of undecoded bytes.
	 * @param inputBufferLen Length of buffer of undecoded bytes.
	 * @param outputBuffer Resulting buffer of manchester-decoded bytes.
	 * @param outputBufferLen Length of resulting buffer of manchester-decoded bytes.
	 *
	 * @return Length of resulting buffer of manchester-decoded bytes or 0, if decoding failed.
	 */
	uint8_t decode(uint8_t *inputBuffer, size_t inputBufferLen, uint8_t *outputBuffer, size_t& outputBufferLen);

};

#endif /* MANCHESTER_HPP_ */
