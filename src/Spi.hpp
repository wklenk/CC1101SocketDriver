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

#ifndef SPI_HPP_
#define SPI_HPP_

#include <stdint.h>
#include <stddef.h>

class Spi {
private:
	uint8_t bits;
	uint32_t speed;

	int fd_spi;

public:
	uint8_t readSingleByte(const uint8_t address, uint8_t& value);
	uint8_t readBurst(const uint8_t address, uint8_t buffer[], const size_t nbytes);
	uint8_t readStrobe(const uint8_t address);

	uint8_t writeSingleByte(const uint8_t address, const uint8_t value);
	uint8_t writeBurst(const uint8_t address, const uint8_t buffer[], const size_t nbytes);
};



#endif /* SPI_HPP_ */
