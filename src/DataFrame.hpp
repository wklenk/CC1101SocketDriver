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

#ifndef DATAFRAME_HPP_
#define DATAFRAME_HPP_

#include "Spi.hpp"

/**
 * Decodes/Encodes the data in frames identical to the RFBee firmware.
 * The firmware of the RFBee uses the following data frame format:
 *
 * Byte 0: Number of following bytes (n).
 * Byte 1: Destination address
 * Byte 2: Source address
 * Byte 3 to Byte n: Payload
 * Byte n+1: RSSI (Received Signal Strength Indicator)
 * Byte n+2: LQI (Link Quality Indicator)
 */
class DataFrame {

	Spi *spi;

public:
	DataFrame(Spi* spi);

	int receive(uint8_t buffer[], size_t& nbytes, uint8_t& srcAddress, uint8_t& destAddress, uint8_t& rssi , uint8_t&  lqi);
};


#endif /* DATAFRAME_HPP_ */
