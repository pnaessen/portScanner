/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portScanner.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 08:02:22 by pnaessen          #+#    #+#             */
/*   Updated: 2025/08/28 10:45:19 by pnaessen         ###   ########lyon.fr   */
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

void cleanupConnectionData(ConnectionData& data) {
	
	if(data.sockaddr)
		delete data.sockaddr;
	if(data.socketPoll)
		delete data.socketPoll;
	if(data.socketFd != -1)
		close(data.socketFd);
}

PortStatus handle_async_connect(ConnectionData& data) {

	
	int pollStatus = poll(data.socketPoll, 1, 3000);
	if(pollStatus == 1) {
		int error;
		socklen_t len = sizeof(error);
		getsockopt(data.socketFd, SOL_SOCKET, SO_ERROR, &error, &len);
		if(error == 0) {
			return PORT_OPEN;
		}
	}
	else if (pollStatus <= 0) {
			return PORT_FILTERED;
		}
	return PORT_FILTERED;
}

PortStatus test_port(const std::string& ip, const int port) {
	
	struct ConnectionData data;
	
	
	data.socketFd = socket(AF_INET,SOCK_STREAM, 0);
	data.socketPoll =  setupPoll(data.socketFd);
	data.sockaddr = setupSocket(ip, port);
	
	fcntl(data.socketFd, F_SETFL, O_NONBLOCK);
	
	int status = connect(data.socketFd, (struct sockaddr *)data.sockaddr, sizeof(*data.sockaddr));
	if(status < 0 && errno == EINPROGRESS) {
		PortStatus result = handle_async_connect(data);
		cleanupConnectionData(data);
		return result;
	}
	else if(status == 0) {
		cleanupConnectionData(data);
		return PORT_OPEN;
	}
	cleanupConnectionData(data);
	return PORT_FILTERED;
}

