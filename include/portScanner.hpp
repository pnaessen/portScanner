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

	constexpr int DEFAULT_TIMEOUT_MS = 1500;
	constexpr int DEFAULT_START_PORT = 1;
	constexpr int DEFAULT_END_PORT = 65000;

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

	
	class PortScanner {
		private:
			std::string _targetIp;
			int _timeoutMs;
			int _threadCount;
		
			sockaddr_in* setupSocket(const std::string& ip, int port);
			ConnectionData setupConnection(int port);
			pollfd* setupPoll(int socketFd);
			std::string checkIpValid(const std::string& ip);


			void cleanupConnectionData(ConnectionData& data);
			
			PortStatus testSinglePort(int port);
			PortStatus handleAsyncConnect(ConnectionData& data);

			std::vector<std::pair<int, int>> calculateThreadDistribution(int start, int end);
			void scanPortRange(int start, int end, std::vector<PortResult>& results);

		public:
			PortScanner(const std::string& target);
			std::vector<PortResult> scanRange(int startPort, int endPort);
			
			// std::vector<PortResult> synScanRange(int startPort, int endPort);
		};

#endif