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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "SocketServer.hpp"
#include "Spi.hpp"
#include "Device.hpp"
#include "Gpio.hpp"
#include "RFBeeDataFrame.hpp"
#include "RawDataFrame.hpp"
#include "DateTime.hpp"
#include "VariableLengthModeProtocol.hpp"
#include "Protocol.hpp"
#include "RegConfigurationHR80.hpp"
#include "RegConfigurationProfile0_27MHz.hpp"

const int PORT = 50000;

int main(int argc, char** argv) {

	// Set up SPI interface
	Spi spi("/dev/spidev0.0", 8, 5 * 1000 * 1000);

	// Set up GPIO Pin 25 as input pin.
	Gpio gpio("25");
	gpio.unexportPin();
	sleep(1);
	gpio.exportPin();
	gpio.setPinDirection(Gpio::DIRECTION_IN);

	VariableLengthModeProtocol protocol(&spi, &gpio);

	// Select a data frame format
	RFBeeDataFrame dataFrame(&protocol);
	//RawDataFrame dataFrame(&protocol);

	// Set up the RF module
	Device device(&spi, &gpio, &dataFrame);
	device.reset();
	//device.configureRegisters(new RegConfigurationHR80());
	device.configureRegisters(new RegConfigurationProfile0_27MHz());

	SocketServer serverSocket(&device);

	serverSocket.open(PORT);

	while (true)
	{
		DateTime::print();
		printf("Accepting incoming connections on port %d ...\n", PORT);
		serverSocket.acceptConnection();
	}

	serverSocket.closeConnection();

	return EXIT_SUCCESS;
}

