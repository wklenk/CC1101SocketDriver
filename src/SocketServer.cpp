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
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "SocketServer.hpp"

static const char* ERROR_LINE_TOO_LONG = "ERROR: Line too long.\n";

void error(const char *msg) {
	perror(msg);
	exit(1);
}

/**
 * Open the server socket.
 */
void SocketServer::open(int portno)
{
	struct sockaddr_in serv_addr;

	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}

	listen(this->sockfd, 1); // Allowing a queue of up to 1 pending connection.
}

void SocketServer::acceptConnection()
{
	int newsockfd;
	socklen_t clilen;

	struct sockaddr_in cli_addr;
	clilen = sizeof(cli_addr);

	newsockfd = accept(this->sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");

	const int MAX_LINE_LENGTH = 10; // TODO: Make larger
	char line[MAX_LINE_LENGTH];
	int i = 0;

	char c;
	while (read(newsockfd, &c, 1) > 0) {
		if (c != '\n' && c != '\r') {
			line[i++] = c;

			if (i >= (MAX_LINE_LENGTH-1)) {
				write(newsockfd, ERROR_LINE_TOO_LONG, strlen(ERROR_LINE_TOO_LONG));
				break;
			}
		} else {
			line[i++] = 0; // Terminate the string
			write(newsockfd, line, i);

			if (strncasecmp("quit", line, 4) == 0) {
				break;
			}

			i = 0;
		}
	}

	close(newsockfd);
}

/**
 * Close the server socket
 */
void SocketServer::closeConnection()
{
	if (close(this->sockfd) < 0) {
		error("Error closing socket.");
	}
}

