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
 * Represents a GPIO pin,
 * using the sysfs userspace interface provided by the kernel.
 * 
 * See https://www.kernel.org/doc/Documentation/gpio.txt for details.
 */
class Gpio {
private:
	/** Name of the GPIO pin */
	const char* pin;

public:
	Gpio(const char* pin);
	~Gpio();

	/**
	 * Ask the kernel to export control of this GPIO pin to userspace.
	 */
	void exportPin();
	
	/**
	 * Reverses the effect of exporting control of this GPIO pin to userspace.
	 */
	void unexportPin();
	
	/**
	 * Define the direction of the GPIO pin: "in" or "out".
	 */
	void setPinDirection(const char* direction);
	
	/**
	 * Define the GPIO pin to generate interrupts on changes of its value.
	 * Possible values are "none", "rising", "falling" or "both".
	 */
	void setPinEdge(const char* edge);

	/**
	 * Get the current value of the GPIO pin.
	 */
	void getPinValue(void* value, size_t nbytes);
	
	/**
	 * If the GPIO pin was configured to generate interrupts (see the
	 * description of "edge"), you can use this method to wait for the edge
	 * condition to happen. If 0 is returned, the edge condition did not
	 * happen within the specified timeout.
	 */
	int waitForPinValueChange(int timeout_millis);
};


#endif /* GPIO_HPP_ */
