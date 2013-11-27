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

#ifndef GPIO_HPP_
#define GPIO_HPP_

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

static const char* DIRECTION_IN = "in";
static const char* DIRECTION_OUT = "out";

static const char* EDGE_NONE = "none";
static const char* EDGE_RISING = "rising";
static const char* EDGE_FALLING = "falling";
static const char* EDGE_BOTH = "both";

/**
 * Provides methods for dealing with a GPIO pin acting as input.
 * This pin is used to signal that incoming data is available in
 * the CC1101's RX FIFO buffer.
 */
class Gpio {
private:
	const char* pin;

public:
	Gpio(const char* pin);
	~Gpio();

	void exportPin();
	void unexportPin();
	void setPinDirection(const char* direction);
	void setPinEdge(const char* edge);

	void getPinValue(void* value, size_t nbytes);
	int waitForPinValueChange(int timeout_millis);
};


#endif /* GPIO_HPP_ */
