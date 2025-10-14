/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/10/14 17:37:43 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

pollfd* PortScanner::setupPoll(int socketFd) {

	pollfd* socketPoll = new pollfd;

	socketPoll->fd = socketFd;
	socketPoll->events = POLLIN;

	return socketPoll;
}


sockaddr_in* PortScanner::setupSocket(const std::string& ip, int port) {

	sockaddr_in* sockaddr = new sockaddr_in;


	sockaddr->sin_port = htons(port);
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_addr.s_addr = inet_addr(ip.c_str());
	return sockaddr;
}

int PortScanner::createRawSocket() {

	int socketFd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if(socketFd < 0) {
		throw std::runtime_error("Fail socket creation ");
	}

	int one = 1;
	if (setsockopt(socketFd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
    	throw std::runtime_error("Fail settings socket ");
	}

    return socketFd;
}
