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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "AddressSpace.hpp"
#include "DateTime.hpp"

#include "Device.hpp"


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

int Device::blockingRead(int otherFd, int timeoutMillis) {

	assert(otherFd >= 0);

	spi->readStrobe(STROBE_SFRX); // Flush the RX FIFO
	spi->readStrobe(STROBE_SRX);  // Enable RX mode

	DateTime::print();
	printf("Wait for pin value change.\n");

	int rc = gpio->waitForPinValueChange(timeoutMillis, otherFd);
	if ( rc > 0) {
		// GPIO input pin raised -> data available
		assert(this->dataFrame != NULL);
		if (this->dataFrame->receive() < 0) {
			// CRC error

			DateTime::print();
			printf("Invalid incoming frame.\n");

			return 0; // TODO: Really same as timeout?
		}

		return rc;

	} else if (rc == 0) {
		// Timeout. Nothing received.

		DateTime::print();
		printf("Timeout.\n");
		return rc;
	} else {
		// Some event on other file descriptor (socket fd)

		DateTime::print();
		printf("Event on socket.\n");

		return rc;
	}
}
