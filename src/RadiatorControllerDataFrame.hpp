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

#ifndef RADIATORCONTROLLERDATAFRAME_HPP_
#define RADIATORCONTROLLERDATAFRAME_HPP_

#include <stdint.h>
#include <stddef.h>

#include "IDataFrame.hpp"
#include "Protocol.hpp"

/**
 * Custom data frame that does conversions of the data.
 *
 */
class RadiatorControllerDataFrame : public IDataFrame {

public:
	static const int FIFO_LENGTH = 64;

	uint8_t buffer[FIFO_LENGTH];
	uint8_t len;

	/** Received Signal Strength Indicator */
	uint8_t rssi;

	/**
	 * Link Quality Indicator. LQI is best used as a relative measurement
	 * of the link quality (a low value indicates a better link than what a
	 * high value does), since the value is dependent on the modulation format.
	 */
	uint8_t lqi;

	RadiatorControllerDataFrame(Protocol* protocol);

	virtual ~RadiatorControllerDataFrame() {};

	/**
	 * Returns 0 if a valid data frame could be read from the RX FIFO
	 * and decoded successfully.
	 * Returns -1 on error.
	 */
	virtual int receive();

	virtual int transmit();

	/**
	 * Writes the data frame to a file descriptor.
	 */
	virtual void writeToSocket(int fd);
};


#endif /* RADIATORCONTROLLERDATAFRAME_HPP_ */
