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

#ifndef IDATAFRAME_HPP_
#define IDATAFRAME_HPP_

#include <stdint.h>
#include <stddef.h>

#include "Protocol.hpp"

/**
 * Interface for all DataFrame implementation.
 *
 * A data frame provides a way to structure the data bytes received into
 * custom fields. When transmitting data bytes, the value of the data frame's
 * fields are collected and transformed into a buffer of bytes.
 */
class IDataFrame {

protected:
	Protocol* protocol;

public:

	IDataFrame(Protocol* protocol) { this->protocol = protocol; };
	virtual ~IDataFrame() {};

	/**
	 * Receive data bytes over the air and structure them into fields.
	 * Only call this method if there is some data in the RX FIFO.
	 *
	 *
	 * Returns 0 if a valid data frame could be read from the RX FIFO
	 * and decoded successfully.
	 * Returns -1 on error.
	 */
	virtual int receive() = 0;

	/**
	 * Collect the data frame's fields, transform them into a buffer of
	 * bytes and transmit the byte buffer over the air.
	 */
	virtual int transmit() = 0;


	/**
	 * Writes the data frame to a file descriptor in a custom format.
	 */
	virtual void writeToSocket(int fd) = 0;
};


#endif /* IDATAFRAME_HPP__ */
