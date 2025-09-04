/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portScanner.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pn <pn@student.42lyon.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 08:02:22 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/04 20:30:30 by pn               ###   ########lyon.fr   */
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
	
	
	sockaddr->sin_port = htons(port);
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_addr.s_addr = inet_addr(ip.c_str());
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

PortStatus handleAsyncConnect(ConnectionData& data) {

	
	int pollStatus = poll(data.socketPoll, 1, DEFAULT_TIMEOUT_MS);
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

PortStatus testPort(const std::string& ip, const int port) {
	
	struct ConnectionData data;
	
	
	data.socketFd = socket(AF_INET,SOCK_STREAM, 0);
	data.socketPoll =  setupPoll(data.socketFd);
	data.sockaddr = setupSocket(ip, port);
	if(!data.sockaddr) {
		cleanupConnectionData(data);
		return PORT_CLOSED;
	}
	
	fcntl(data.socketFd, F_SETFL, O_NONBLOCK);
	
	int status = connect(data.socketFd, (struct sockaddr *)data.sockaddr, sizeof(*data.sockaddr));
	if(status < 0 && errno == EINPROGRESS) {
		PortStatus result = handleAsyncConnect(data);
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

std::string checkIpValid(const std::string& ip) {
	
	sockaddr_in sockaddr;
	struct addrinfo *addinfo;
	char ipstr[INET_ADDRSTRLEN] = {0};
	
	int status = getaddrinfo(ip.c_str(), NULL, NULL, &addinfo);
	if (status == 0) {
		struct sockaddr_in* addr_in = (struct sockaddr_in*)addinfo->ai_addr;
		sockaddr.sin_addr.s_addr = addr_in->sin_addr.s_addr;
		inet_ntop(AF_INET, &sockaddr.sin_addr, ipstr, sizeof(ipstr));
		freeaddrinfo(addinfo);
		return std::string(ipstr);
	}
	else {
		sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
		if(sockaddr.sin_addr.s_addr == INADDR_NONE) {
			throw std::out_of_range("DNS resolution failed :") ;
		}
		else {
			return ip;
		}
	}
}

void workerThread(const std::string& ip,int start, int end, std::vector<PortResult>& result ) {
	
	result.reserve(end - start + 1); 
	for(int port = start; port <= end; port++) {
		PortResult res;
		res.port = port;
		res.status = testPort(ip, port);
		result.push_back(res);
	}
	return ;
	
}