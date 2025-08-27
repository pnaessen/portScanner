/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portScanner.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 08:02:22 by pnaessen          #+#    #+#             */
/*   Updated: 2025/08/27 12:51:57 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

pollfd* setupPoll(int socketFd) {
	
	pollfd* socketPoll = new pollfd;

	socketPoll->fd = socketFd;

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
	
	int status = connect(socketFd, (struct sockaddr *)sockaddr, sizeof(sockaddr));
	if(status < 0 && errno == EINPROGRESS) {
		pollfd* socketPoll = setupPoll(socketFd);
		int pollStatus = poll(socketPoll, POLLOUT, 3000);
		delete socketPoll;
		delete sockaddr;
		if(pollStatus < 0) {
			return PORT_FILTERED;
		}
		return PORT_OPEN;
	}
	else if(status == 0) {
		delete sockaddr;
		return PORT_OPEN;
	}
	else if(status < 0) {
		delete sockaddr;
		return PORT_CLOSED;
	}
	
	delete sockaddr;
	return PORT_FILTERED;
}