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

#ifndef DEVICE_HPP_
#define DEVICE_HPP_

#include "Spi.hpp"
#include "Gpio.hpp"
#include "RegConfiguration.hpp"
#include "DataFrame.hpp"

/**
 * Represents a CC1101 based RF communication module.
 */
class Device {
	Spi* spi;
	Gpio* gpio;

public:
	DataFrame* dataFrame;

	Device(Spi* spi, Gpio* gpio, DataFrame* dataFrame);

	void reset();
	void configureRegisters(RegConfiguration* configuration);

	int blockingRead(int otherFD, int timeoutMillis);
};

#endif /* DEVICE_HPP_ */
