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
	#include <sstream>
	#include <atomic>
	#include <thread>
	#include <optional>




	// TODO: Add raw socket support includes
	// TODO: Consider adding libpcap for advanced packet crafting

	constexpr int DEFAULT_TIMEOUT_MS = 1500;
	constexpr int DEFAULT_START_PORT = 1;
	constexpr int DEFAULT_END_PORT = 65535;

	enum PortStatus {
		PORT_OPEN,
		PORT_CLOSED,
		PORT_FILTERED
		// TODO: Add more status types for raw socket scanning
		// TODO: Add PORT_UNFILTERED, PORT_OPEN_FILTERED states
	};

	struct ConnectionData {
   	 	pollfd* socketPoll;
   		sockaddr_in* sockaddr;
   		int socketFd;
	};

	struct PortResult {
    	int port;
    	PortStatus status;
		// TODO: Add service detection information
		// TODO: Add response time measurement
		// TODO: Add banner grabbing results
	};


	class PortScanner {
		private:
			std::string _targetIp;
			int _timeoutMs;
			int _threadCount;
			bool _noProgress;

			sockaddr_in* setupSocket(const std::string& ip, int port);
			ConnectionData setupConnection(int port);
			pollfd* setupPoll(int socketFd);
			std::string checkIpValid(const std::string& ip);


			void cleanupConnectionData(ConnectionData& data);

			PortStatus testSinglePort(int port);
			PortStatus handleAsyncConnect(ConnectionData& data);

			std::vector<std::pair<int, int>> calculateThreadDistribution(int start, int end);
			void scanPortRange(int start, int end, std::vector<PortResult>& results, std::atomic<int>& progress);
			void monitorProgress(std::atomic<bool>* done, std::vector<std::atomic<int>>* progress, int totalPorts, std::chrono::steady_clock::time_point startTime);


		public:
			PortScanner(const std::string& target, bool flag);
			std::vector<PortResult> scanRange(int startPort, int endPort);

			// TODO: Implement SYN scan using raw sockets
			// std::vector<PortResult> synScanRange(int startPort, int endPort);

			// TODO: Add UDP scanning capability
			// std::vector<PortResult> udpScanRange(int startPort, int endPort);

			// TODO: Add FIN scan implementation
			// TODO: Add NULL scan implementation
			// TODO: Add Xmas scan implementation
		};

#endif