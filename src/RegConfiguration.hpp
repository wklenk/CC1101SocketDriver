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

#ifndef REGCONFIGURATION_HPP_
#define REGCONFIGURATION_HPP_

#include <stdint.h>
#include <stddef.h>

/**
 * Abstract CC1101 register configuration.
 * All 47 settings from 0x00 to 0x2E need to be provided.
 */
class RegConfiguration {
public:
	virtual const uint8_t* getValues() = 0;
};


#endif /* REGCONFIGURATION_HPP_ */
