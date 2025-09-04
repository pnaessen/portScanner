/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portScanner.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:57:39 by pnaessen          #+#    #+#             */
/*   Updated: 2025/08/27 07:58:11 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PORTSCANNER_HPP
# define PORTSCANNER_HPP

	#include <string>
	#include <iostream>
	#include <poll.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
    #include <netdb.h>
	#include <chrono>
	#include <vector>
	#include <future>

	constexpr int DEFAULT_TIMEOUT_MS = 3000;
	constexpr int DEFAULT_START_PORT = 1;
	constexpr int DEFAULT_END_PORT = 100;

	enum PortStatus {
		PORT_OPEN,
		PORT_CLOSED,
		PORT_FILTERED
	};

	struct ConnectionData {
   	 	pollfd* socketPoll;
   		sockaddr_in* sockaddr;
   		int socketFd;
	};

	struct PortResult {
    	int port;
    	PortStatus status;
	};

	sockaddr_in* setupSocket(const std::string& ip, int port);
	pollfd* setupPoll(int socketFd);
	PortStatus testPort(const std::string& ip, const int port);
	void cleanupConnectionData(ConnectionData& data);
	PortStatus handleAsyncConnect(int socketFd);
	std::string checkIpValid(const std::string& ip);
	void workerThread(const std::string& ip,int start, int end, std::vector<PortResult>& result );

#endif