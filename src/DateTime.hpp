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

#ifndef DATETIME_HPP_
#define DATETIME_HPP_

#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include <sys/time.h>


class DateTime {

public:
	static void print() {
		char            fmt[64], buf[64];
		struct timeval  tv;
		struct tm       *tm;

		gettimeofday(&tv, NULL);
		if((tm = localtime(&tv.tv_sec)) != NULL)
		{
			strftime(fmt, sizeof fmt, "%Y-%m-%d %H:%M:%S.%%06u", tm);
			snprintf(buf, sizeof buf, fmt, tv.tv_usec);
			printf("%s ", buf);
		}
	};
};


#endif /* DATETIME_HPP_ */
