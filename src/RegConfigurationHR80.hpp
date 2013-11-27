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

#ifndef REGCONFIGURATIONHR80_HPP_
#define REGCONFIGURATIONHR80_HPP_

#include <stdint.h>
#include <stddef.h>

#include "RegConfiguration.hpp"

/**
 * Register configuration for Honeywell HR80 radiator controller.
 */
class RegConfigurationHR80 : public RegConfiguration {
private:
	static const uint8_t register_configuration[];

public:
	virtual const uint8_t* getValues() {
		return this->register_configuration;
	}
};


#endif /* REGCONFIGURATIONHR80_HPP_ */
