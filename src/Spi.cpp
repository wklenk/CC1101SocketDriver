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

#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "DateTime.hpp"

#include "Spi.hpp"

Spi::Spi(const char* device, uint8_t bits, uint32_t speed) {

	this->bits = bits;
	this->speed = speed;

	this->fd_spi = open(device, O_RDWR);
	if (this->fd_spi < 0) {
		fprintf(stderr, "Can't open device %s\n", device);
		exit(1);
	}
}

Spi::~Spi() {
	if (this->fd_spi >= 0) {
		close(this->fd_spi);
	}
}

uint8_t Spi::readSingleByte(const uint8_t address, uint8_t& value) {

	static const int LEN = 2;
	uint8_t tx[LEN] = {0x80 | address, 0x00};
	uint8_t rx[LEN];

	struct spi_ioc_transfer tr;
	memset(&tr, 0, sizeof tr);

	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = LEN;
	tr.speed_hz = this->speed;
	tr.bits_per_word = this->bits;

	int rc = ioctl(this->fd_spi, SPI_IOC_MESSAGE(1), &tr);
	if (rc < 0) {
		perror("SPI single byte read failed");
		abort();
	}

	value = rx[1];

	DateTime::print();
	printf("read single byte address=0x%.2X status=0x%.2X value=0x%.2X\n", address, rx[0], value);

	// CHIP_RDYn (Bit 7)
	// Stays high until power and crystal have stabilized.
	// Should always be low when using the SPI interface.
	assert((rx[0] & 0x80) == 0);

	return rx[0];
}

uint8_t Spi::readBurst(const uint8_t address, uint8_t buffer[], const size_t nbytes) {

	const int LEN = 1 + nbytes;
	uint8_t tx[LEN];
	memset(tx, 0x00, LEN);
	tx[0] = 0xC0 | address;

	uint8_t rx[LEN];
	memset(rx, 0x00, LEN);

	struct spi_ioc_transfer tr;
	memset(&tr, 0, sizeof tr);

	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = LEN;
	tr.speed_hz = this->speed;
	tr.bits_per_word = this->bits;

	int rc = ioctl(fd_spi, SPI_IOC_MESSAGE(1), &tr);
	if (rc < 0) {
		perror("SPI burst read failed");
		abort();
	}

	memcpy(buffer, rx+1, nbytes);

	/*
	DateTime::print();
	printf("read burst       address=0x%.2X status=0x%.2X value=", address, rx[0]);

	for (int i=0 ; i<nbytes ; i++) {
		printf("0x%.2X ", buffer[i]);
	}
	printf("\n");
	*/

	// CHIP_RDYn (Bit 7)
	// Stays high until power and crystal have stabilized.
	// Should always be low when using the SPI interface.
	assert((rx[0] & 0x80) == 0);

	return rx[0];
}

uint8_t Spi::readStrobe(const uint8_t address)
{
	uint8_t tx[1] = {0x80 | address};
	uint8_t rx[1];

	struct spi_ioc_transfer tr;
	memset(&tr, 0, sizeof tr);

	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = 1;
	tr.speed_hz = this->speed;
	tr.bits_per_word = this->bits;

	int rc = ioctl(fd_spi, SPI_IOC_MESSAGE(1), &tr);
	if (rc < 0) {
		perror("SPI strobe read failed");
		abort();
	}

	/*
	DateTime::print();
	printf("read strobe      address=0x%.2X status=0x%.2X\n", address, rx[0]);
	*/

	return rx[0];
}

uint8_t Spi::writeSingleByte(const uint8_t address, const uint8_t value) {

	const int LEN = 2;
	uint8_t tx[LEN];
	tx[0] = 0x40 | address;
	tx[1] = value;

	uint8_t rx[LEN];
	memset(rx, 0x00, LEN);

	struct spi_ioc_transfer tr ;
	memset(&tr, 0, sizeof tr);

	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = LEN;
	tr.speed_hz = this->speed;
	tr.bits_per_word = this->bits;

	int rc = ioctl(fd_spi, SPI_IOC_MESSAGE(1), &tr);
	if (rc < 0) {
		perror("SPI single byte write failed");
		abort();
	}

	return rx[0];
}

uint8_t Spi::writeBurst(const uint8_t address, const uint8_t buffer[], const size_t nbytes) {

	const int LEN = 1 + nbytes;
	uint8_t tx[LEN];
	tx[0] = 0x40 | address;
	memcpy(tx + 1, buffer, nbytes);

	uint8_t rx[LEN];
	memset(rx, 0x00, LEN);

	struct spi_ioc_transfer tr ;
	memset(&tr, 0, sizeof tr);

	tr.tx_buf = (unsigned long) tx;
	tr.rx_buf = (unsigned long) rx;
	tr.len = LEN;
	tr.speed_hz = this->speed;
	tr.bits_per_word = this->bits;

	int rc = ioctl(fd_spi, SPI_IOC_MESSAGE(1), &tr);
	if (rc < 0) {
		perror("SPI burst write failed");
		abort();
	}

	return rx[0];
}



