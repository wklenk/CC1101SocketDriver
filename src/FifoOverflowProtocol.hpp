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

#ifndef FIFOOVERFLOWPROTOCOL_HPP_
#define FIFOOVERFLOWPROTOCOL_HPP_

#include "Spi.hpp"
#include "Protocol.hpp"

/**
 * This protocol is used when you intentionally let overflow the CC1101's
 * receive FIFO and then just read out all data in the FIFO.
 * As the receive FIFO has a length of just 64 bytes, this is also
 * the maximum message length.
 */
class FifoOverflowProtocol : public Protocol {

public:
	static const int FIFO_LENGTH = 64;

	FifoOverflowProtocol(Spi* spi);

	/**
	 * Note: RSSI and LQI are appended to the end of the message.
	 */
	int receive(uint8_t buffer[], size_t& nbytes);
	int transmit(const uint8_t buffer[], size_t nbytes);

private:
	Spi* spi;
	uint8_t fifo[FIFO_LENGTH];
};

#endif /* FIFOOVERFLOWPROTOCOL_HPP_ */
