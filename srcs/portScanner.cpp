/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portScanner.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 08:02:22 by pnaessen          #+#    #+#             */
/*   Updated: 2025/08/28 09:24:24 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

pollfd* setupPoll(int socketFd) {
	
	pollfd* socketPoll = new pollfd;

	socketPoll->fd = socketFd;
	socketPoll->events = POLLOUT;

	return socketPoll;
}

sockaddr_in* setupSocket(const std::string& ip, int port) {
	
	sockaddr_in* sockaddr = new sockaddr_in;
	
	//struct addrinfo * addinfo = setupAddr();
	
	sockaddr->sin_port = htons(port);
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_addr.s_addr = inet_addr("8.8.8.8");
	(void)ip;
	//sockaddr.sin_addr = getaddrinfo(ip.c_str(), NULL, NULL, NULL);
	
	return sockaddr;
}

PortStatus test_port(const std::string& ip, const int port) {
	
	struct sockaddr_in *sockaddr = setupSocket(ip, port);
	
	
	int socketFd = socket(AF_INET,SOCK_STREAM, 0);
	fcntl(socketFd, F_SETFL, O_NONBLOCK);
	
	int status = connect(socketFd, (struct sockaddr *)sockaddr, sizeof(*sockaddr));
	if(status < 0 && errno == EINPROGRESS) {
		pollfd* socketPoll = setupPoll(socketFd);
		int pollStatus = poll(socketPoll, 1, 3000);
		if(pollStatus == 1) {
			int error;
			socklen_t len = sizeof(error);
			getsockopt(socketFd, SOL_SOCKET, SO_ERROR, &error, &len);
			delete socketPoll;
			delete sockaddr;
			close(socketFd);
			if(error == 0) {
				return PORT_OPEN;
			}
			else {
				return PORT_FILTERED;
			}
		}
		else if (pollStatus <= 0) {
				//std::cerr << "Timeout" << std::endl;
				delete socketPoll;
				delete sockaddr;
				close(socketFd);
				return PORT_FILTERED;
			}
	}
	else if(status == 0) {
		
		delete sockaddr;
		close(socketFd);
		return PORT_OPEN;
	}
	delete sockaddr;
	close(socketFd);
	return PORT_FILTERED;
}
