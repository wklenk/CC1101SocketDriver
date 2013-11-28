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

#include "Device.hpp"

#include "AddressSpace.hpp"

Device::Device(Spi* spi, Gpio* gpio, DataFrame* dataFrame) {
	this->spi = spi;
	this->gpio = gpio;
	this->dataFrame = dataFrame;
}

void Device::reset() {
	this->spi->readStrobe(STROBE_SRES);
}

void Device::configureRegisters(RegConfiguration* configuration) {
	// Number of registers: 0x2E
	// Configure all registers in a single write burst.
	this->spi->writeBurst(0x00, configuration->getValues(), 0x2E);
}

DataFrame* Device::blockingRead(int timeoutMillis) {

	spi->readStrobe(STROBE_SFRX); // Flush the RX FIFO
	spi->readStrobe(STROBE_SRX);  // Enable RX mode

	if (gpio->waitForPinValueChange(timeoutMillis) != 0) {
		// GPIO input pin raised -> data available

		if (this->dataFrame->receive() < 0) {
			// CRC error
			return NULL;
		}

		// Return the data frame
		return this->dataFrame;

	} else {
		// Timeout. Nothing received.
		return NULL;
	}
}
