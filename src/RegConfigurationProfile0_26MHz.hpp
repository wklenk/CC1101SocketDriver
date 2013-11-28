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

#ifndef REGCONFIGURATIONPROFILE0_26MHZ_HPP_
#define REGCONFIGURATIONPROFILE0_26MHZ_HPP_

#include <stdint.h>
#include <stddef.h>

#include "RegConfiguration.hpp"

/**
 * Register configuration Profile 0 for 26MHz antenna crystal.
 *
 * Sync word qualifier mode = 30/32 sync word bits detected
 * CRC autoflush = false
 * Channel spacing = 199.951172
 * Data format = Normal mode
 * Data rate = 76.767
 * RX filter BW = 232.142857
 * PA ramping = false
 * Preamble count = 4
 * Whitening = false
 * Address config = No address check
 * Carrier frequency = 914.999969
 * Device address = 0
 * TX power = unknown
 * Manchester enable = false
 * CRC enable = true
 * Deviation = 31.738281
 * Packet length mode = Variable packet length mode. Packet length configured by the first byte after sync word
 * Packet length = 255
 * Modulation format = 2-FSK
 * Base frequency = 914.999969
 * Modulated = true
 * Channel number = 0
 */
class RegConfigurationProfile0_26MHz : public RegConfiguration {
private:
	static const uint8_t register_configuration[];

public:
	virtual const uint8_t* getValues() {
		return this->register_configuration;
	}
};


#endif /* REGCONFIGURATIONPROFILE0_26MHZ_HPP_ */
