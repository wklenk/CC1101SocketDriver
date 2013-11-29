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

#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

#include "Gpio.hpp"

const char* Gpio::DIRECTION_IN = "in";
const char* Gpio::DIRECTION_OUT = "out";

const char* Gpio::EDGE_NONE = "none";
const char* Gpio::EDGE_RISING = "rising";
const char* Gpio::EDGE_FALLING = "falling";
const char* Gpio::EDGE_BOTH = "both";

Gpio::Gpio(const char* pin) {
	this->pin = pin;
}

Gpio::~Gpio() {
	this->unexportPin();
}

/**
 * Ask the kernel to export control of a GPIO to userspace by writing its number to file
 * /sys/class/gpio/export.
 */
void Gpio::exportPin() {
	static const char* fn = "/sys/class/gpio/export";
	int fd = open(fn, O_WRONLY);
	if(fd < 0) {
		perror(fn);
		exit(1);
	}

	size_t rc = write(fd, this->pin, strlen(this->pin));
	if(rc != strlen(pin)) {
		perror("write export");
		exit(1);
	}

	close(fd);
}

/**
 * Reverses the effect of exporting a GPIO to userspace by writing its number to file
 * /sys/class/gpio/unexport.
 */
void Gpio::unexportPin() {
	static const char* fn = "/sys/class/gpio/unexport";
	int fd = open(fn, O_WRONLY);
	if(fd < 0) {
		perror(fn);
		exit(1);
	}

	size_t rc = write(fd, this->pin, strlen(this->pin));
	if(rc != strlen(pin)) {
		perror("write unexport");
		exit(1);
	}
	
	close(fd);
}

/**
 * Sets the direction of a GPIO pin.
 * Reads as either "in" or "out".  This value may normally be written.  
 * Writing as "out" defaults to initializing the value as low.  
 * To ensure glitch free operation, values "low" and "high" may be written to
 * configure the GPIO as an output with that initial value.
 */
void Gpio::setPinDirection(const char* direction) {

	const int BUFSIZE = 64;
	char fn[BUFSIZE];
	snprintf(fn, BUFSIZE, "/sys/class/gpio/gpio%s/direction", this->pin);
	int fd = open(fn, O_WRONLY);
	if(fd < 0)  {
		perror(fn);
		exit(1);
	}

	size_t rc = write(fd, direction, strlen(direction));
	if(rc != strlen(direction)) {
		perror("write direction");
		exit(1);
	}

	close(fd);
}

/**
 * Sets the signal edge(s) that will make poll(2) on the "value" file return.
 * Reads as either "none", "rising", "falling", or "both". 
 * This file exists only if the pin can be configured as an interrupt generating input pin.
 */
void Gpio::setPinEdge(const char* edge) {

	const int BUFSIZE = 64;
	char fn[BUFSIZE];
	snprintf(fn, BUFSIZE, "/sys/class/gpio/gpio%s/edge", this->pin);
	int fd = open(fn, O_WRONLY);
	if(fd < 0)  {
		perror(fn);
		exit(1);
	}

	size_t rc = write(fd, edge, strlen(edge));
	if(rc != strlen(edge)) {
		perror("write edge");
		exit(1);
	}

	close(fd);
}

/**
 * Read the value of a GPIO pin.
 * Returns either 0 (low) or 1 (high).  
 * 
 * If the pin can be configured as interrupt-generating interrupt
 * and if it has been configured to generate interrupts (see the
 * description of "edge"), you can poll(2) on that file and
 * poll(2) will return whenever the interrupt was triggered. If
 * you use poll(2), set the events POLLPRI and POLLERR. If you
 * use select(2), set the file descriptor in exceptfds. After
 * poll(2) returns, either lseek(2) to the beginning of the sysfs
 * file and read the new value or close the file and re-open it
 * to read the value.
 */
void Gpio::getPinValue(void* value, size_t nbytes) {

	const int BUFSIZE = 64;
	char fn[BUFSIZE];
	snprintf(fn, BUFSIZE, "/sys/class/gpio/gpio%s/value", this->pin);
	int fd = open(fn, O_RDONLY);
	if(fd < 0)  {
		perror(fn);
		exit(1);
	}


	int rc = read(fd, value, nbytes);
	if(rc < 0) {
		perror("read");
		exit(1);
	}

	close(fd);
}

/**
 * If the GPIO pin was configured to generate interrupts (see the
 * description of "edge"), you can use this method to wait for the edge
 * condition to happen.
 * 
 * Method will return with 0 if edge condition did not happen within the
 * specified timeout.
 */
int Gpio::waitForPinValueChange(int timout_millis, int otherFd) {

	const int BUFSIZE = 64;
	char fn[BUFSIZE];
	snprintf(fn, BUFSIZE, "/sys/class/gpio/gpio%s/value", this->pin);
	int fd = open(fn, O_RDONLY);
	if(fd < 0)  {
		perror(fn);
		exit(1);
	}

	// Dummy read to clear the interrupt.
	uint8_t c;
	int rc = read (fd, &c, 1);
	if (rc < 0) {
		perror("read");
		exit(1);
	}

	struct pollfd pl[2];
	pl[0].fd = fd;
	pl[0].events = POLLPRI | POLLERR; // TODO: Documentation says, POLLERR is implicit

	pl[1].fd = otherFd;
	pl[1].events = POLLIN | POLLERR;

	printf("Entering poll\n");
	rc = poll(pl, 2, timout_millis);
	if(rc < 0) {
		perror("poll");
		exit(1);
	}
	printf("Leaving poll rc: %.2X\n", rc);

	close(fd);

	printf("GPIO Pin events: %.2X\n", pl[0].revents);
	printf("Other FD events: %.2X\n", pl[1].revents);

	// Will return 0 in case of timeout
	if (rc == 0) {
		return 0; // Timeout
	}

	if (pl[1].revents > 0) {
		// Data ready to read from the socket or socket was closed.
		return -1;
	}

	return 1; // Data available in RX FIFO
}

