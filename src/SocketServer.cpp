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
#include <arpa/inet.h>

#include "SocketServer.hpp"
#include "RFBeeDataFrame.hpp"
#include "DateTime.hpp"

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

	DateTime::print();
	printf("Incoming connection from %s\n", inet_ntoa(cli_addr.sin_addr));

	for (;;) {

		// We do a blocking read (waiting for incoming RF data), but this method
		// also returns if there is incoming data from the socket.
		int rc = device->blockingRead(newsockfd, 60000);
		if (rc > 0) {
			// Write the data frame to the socket, using
			// the selected output format.
			device->dataFrame->writeToSocket(newsockfd);

		} else if (rc == 0) {
			const char* TIMEOUT = "Timeout\n";
			write(newsockfd, TIMEOUT, strlen(TIMEOUT));
		} else {
			// Something wrong with socket FD
			break; // Leave the loop
		}
	}

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

