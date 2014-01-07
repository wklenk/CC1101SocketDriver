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

#include <stdint.h>
#include <stddef.h>

#include "CC1101Transceiver.hpp"

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
 *
 * Payload is limited to 255 bytes.
 */
class DataFrame {
private:
	CC1101Transceiver* transceiver;

public:

	static const int MAX_PAYLOAD_BYTES = 256;

	uint8_t buffer[MAX_PAYLOAD_BYTES];
	uint8_t len;

	uint8_t srcAddress;
	uint8_t destAddress;

	/** Received Signal Strength Indicator */
	uint8_t rssi;

	/** Format used to write data frame to the socket */
	int outputFormat;

	/**
	 * Link Quality Indicator. LQI is best used as a relative measurement
	 * of the link quality (a low value indicates a better link than what a
	 * high value does), since the value is dependent on the modulation format.
	 */
	uint8_t lqi;

	DataFrame(CC1101Transceiver* transceiver);

	int receive();
	int transmit();


	/**
	 * Writes the data frame to a file descriptor.
	 */
	void writeToSocket(int fd);
};


#endif /* DATAFRAME_HPP_ */
