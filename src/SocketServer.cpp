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
#include "DataFrame.hpp"

static const char* ERROR_LINE_TOO_LONG = "ERROR: Line too long.\n";


SocketServer::SocketServer(Device* device) {
	this->device = device;
	this->sockfd = -1;
}


/**
 * Open the server socket.
 */
void SocketServer::open(int portno)
{
	struct sockaddr_in serv_addr;

	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Opening server socket");
		exit(1);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Binding server socket");
		exit(1);
	}

	// Allowing a queue of up to 1 pending connection.
	if (listen(this->sockfd, 1) < 0 ) {
		perror("Setup server socket connection queue");
		exit(1);
	}; 
}

void SocketServer::acceptConnection()
{
	int newsockfd;
	socklen_t clilen;

	struct sockaddr_in cli_addr;
	clilen = sizeof(cli_addr);

	newsockfd = accept(this->sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) {
		perror("Accepting connection");
		exit(1);
	}

	for (;;) {
		DataFrame* dataFrame = device->blockingRead(60000);
		if (dataFrame != NULL) {
			char line[80];

			snprintf(line, 80, "Status: 0x%.2X\n", dataFrame->status);
			write(newsockfd, line, strlen(line));

			snprintf(line, 80, "Length: %d\n", dataFrame->len);
			write(newsockfd, line, strlen(line));

			snprintf(line, 80, "Source address: %d\n", dataFrame->srcAddress);
			write(newsockfd, line, strlen(line));

			snprintf(line, 80, "Destination address: %d\n", dataFrame->destAddress);
			write(newsockfd, line, strlen(line));

			snprintf(line, 80, "RSSI: %d\n", dataFrame->rssi);
			write(newsockfd, line, strlen(line));

			snprintf(line, 80, "LQI: %d\n", dataFrame->lqi);
			write(newsockfd, line, strlen(line));

			for (int i = 0; i<dataFrame->len; i++) {
				if (!(i % 8)) {
					sprintf(line, "\n");
					write(newsockfd, line, strlen(line));
				}

				sprintf(line, "%.2X ", dataFrame->buffer[i]);
				write(newsockfd, line, strlen(line));
			}
			sprintf(line, "\n");
			write(newsockfd, line, strlen(line));

		} else {
			const char* TIMEOUT = "Timeout\n";
			write(newsockfd, TIMEOUT, strlen(TIMEOUT));
		}
	}


	/*
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
	 */

	if (close(newsockfd) < 0) {
		perror("Closing new socket");
		exit(1);
	}
}

/**
 * Close the server socket
 */
void SocketServer::closeConnection()
{
	if (close(this->sockfd) < 0) {
		perror("Closing server socket");
		exit(1);
	}
	
	this->sockfd = -1;
}

