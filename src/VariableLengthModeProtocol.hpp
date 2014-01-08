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

#ifndef VARIABLELENGTHMODEPROTOCOL_HPP_
#define VARIABLELENGTHMODEPROTOCOL_HPP_

#include "Spi.hpp"
#include "Gpio.hpp"
#include "Protocol.hpp"

/**
 * Uses the CC1101 "Variable Length Mode" to receive and transmit
 * messages up to 255 bytes.
 *
 * The first byte of the message is used as length indicator.
 */
class VariableLengthModeProtocol : public Protocol {

private:
	Spi* spi;
	Gpio* gpio;

public:
	VariableLengthModeProtocol(Spi* spi, Gpio* gpio);

	int receive(uint8_t buffer[], size_t& nbytes);
	int transmit(const uint8_t buffer[], size_t nbytes);
};

#endif /* VARIABLELENGTHMODEPROTOCOL_HPP_ */
