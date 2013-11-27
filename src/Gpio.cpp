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


Gpio::Gpio(const char* pin) {
	this->pin = pin;
}

Gpio::~Gpio() {
	this->unexportPin();
}

void Gpio::exportPin() {
	static const char* fn = "/sys/class/gpio/export";
	int fd = open(fn, O_WRONLY);
	if(fd < 0) {
		perror(fn);
		abort();
	}

	int rc = write(fd, this->pin, strlen(this->pin));
	if(rc != strlen(pin)) {
		fprintf(stderr, "write export");
		exit(1);
	}

	close(fd);
}

void Gpio::unexportPin() {
	static const char* fn = "/sys/class/gpio/unexport";
	int fd = open(fn, O_WRONLY);
	if(fd < 0) {
		fprintf(stderr, fn);
		exit(1);
	}

	write(fd, this->pin, strlen(this->pin));
	close(fd);
}

void Gpio::setPinDirection(const char* direction) {

	const int BUFSIZE = 64;
	char fn[BUFSIZE];
	snprintf(fn, BUFSIZE, "/sys/class/gpio/gpio%s/direction", this->pin);
	int fd = open(fn, O_WRONLY);
	if(fd < 0)  {
		fprintf(stderr, fn);
		exit(1);
	}

	int rc = write(fd, direction, strlen(direction));
	if(rc != strlen(direction)) {
		fprintf(stderr, "write direction");
		exit(1);
	}

	close(fd);
}

void Gpio::setPinEdge(const char* edge) {

	const int BUFSIZE = 64;
	char fn[BUFSIZE];
	snprintf(fn, BUFSIZE, "/sys/class/gpio/gpio%s/edge", this->pin);
	int fd = open(fn, O_WRONLY);
	if(fd < 0)  {
		fprintf(stderr, fn);
		exit(1);
	}

	int rc = write(fd, edge, strlen(edge));
	if(rc != strlen(edge)) {
		fprintf(stderr, "write edge");
		exit(1);
	}

	close(fd);
}

void Gpio::getPinValue(void* value, size_t nbytes) {

	const int BUFSIZE = 64;
	char fn[BUFSIZE];
	snprintf(fn, BUFSIZE, "/sys/class/gpio/gpio%s/value", this->pin);
	int fd = open(fn, O_RDONLY);
	if(fd < 0)  {
		fprintf(stderr, fn);
		exit(1);
	}


	int rc = read(fd, value, nbytes);
	if(rc < 0) {
		fprintf(stderr, "read");
		exit(1);
	}

	close(fd);
}

int Gpio::waitForPinValueChange(int timout_millis) {

	const int BUFSIZE = 64;
	char fn[BUFSIZE];
	snprintf(fn, BUFSIZE, "/sys/class/gpio/gpio%s/value", this->pin);
	int fd = open(fn, O_RDONLY);
	if(fd < 0)  {
		fprintf(stderr, fn);
		exit(1);
	}

	// Dummy read to clear the interrupt.
	uint8_t c;
	read (fd, &c, 1);

	struct pollfd pl;
	pl.fd = fd;
	pl.events = POLLPRI | POLLERR;

	int rc = poll(&pl, 1, timout_millis);
	if(rc < 0) {
		fprintf(stderr, "poll");
		exit(1);
	}

	close(fd);

	return pl.revents; // Will return 0 in case of timeout
}

