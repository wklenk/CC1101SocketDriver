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

#ifndef RAWDATAFRAME_HPP_
#define RAWDATAFRAME_HPP_

#include <stdint.h>
#include <stddef.h>

#include "IDataFrame.hpp"
#include "Protocol.hpp"

/**
 * Decodes/Encodes the data in frames just as a sequence of bytes.
 *
 * Payload is limited to 255 bytes.
 */
class RawDataFrame : public IDataFrame {

public:
	static const int MAX_PAYLOAD_BYTES = 256;

	uint8_t buffer[MAX_PAYLOAD_BYTES];
	uint8_t len;

	RawDataFrame(Protocol* protocol);

	virtual ~RawDataFrame() {};

	/**
	 * Returns 0 if a data frame could be read from the RX FIFO.
	 * Returns -1 on error.
	 */
	virtual int receive();

	virtual int transmit();

	/**
	 * Writes the data frame to a file descriptor.
	 */
	virtual void writeToSocket(int fd);
};


#endif /* RAWDATAFRAME_HPP__ */
