/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portScanner.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 08:02:22 by pnaessen          #+#    #+#             */
/*   Updated: 2025/10/09 11:09:04 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"
#include "cli.hpp"

PortScanner::PortScanner(const std::string& targetIP, bool flag) : _timeoutMs(DEFAULT_TIMEOUT_MS), _noProgress(flag) {

	try {
		_targetIp = checkIpValid(targetIP);
    	_threadCount = std::thread::hardware_concurrency();
		_localIp = getLocalIP("8.8.8.8");
		if(getuid() != 0) {
			throw std::out_of_range("No root privilege ");
		}
		// TODO: Add option to configure timeout
	}
	catch (std::exception &e) {
		std::cerr << e.what();
		throw std::out_of_range(targetIP) ;
	}
}

std::string PortScanner::getLocalIP(const std::string& target_ip) {

    int sockFd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(80);
    target.sin_addr.s_addr = inet_addr(target_ip.c_str());

    connect(sockFd, (struct sockaddr*)&target, sizeof(target));

    struct sockaddr_in local;
    socklen_t len = sizeof(local);
    getsockname(sockFd, (struct sockaddr*)&local, &len);

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &local.sin_addr, ip_str, sizeof(ip_str));

    close(sockFd);
    return std::string(ip_str);
}

void PortScanner::cleanupConnectionData(ConnectionData& data) {

	if(data.sockaddr)
		delete data.sockaddr;
	if(data.socketPoll)
		delete data.socketPoll;
	if(data.socketFd != -1)
		close(data.socketFd);
}

PortStatus PortScanner::handleAsyncConnect(ConnectionData& data) {

	// TODO: Implement packet capture for raw socket responses
	int pollStatus = poll(data.socketPoll, 1, this->_timeoutMs);
	if(pollStatus == 1) {
		int error;
		socklen_t len = sizeof(error);
		getsockopt(data.socketFd, SOL_SOCKET, SO_ERROR, &error, &len);
		if(error == 0) {
			// TODO: Add banner grabbing here for service detection
			return PORT_OPEN;
		}
	}
	else if (pollStatus <= 0) {
		// TODO: Distinguish between filtered and closed ports better
		return PORT_FILTERED;
	}
	// TODO: Add retry logic for uncertain results
	return PORT_FILTERED;
}


std::vector<PortResult> PortScanner::scanRange(int startPort, int endPort) {


	auto threadPortRanges = calculateThreadDistribution(startPort, endPort);

	std::vector<std::vector<PortResult>> allResult(threadPortRanges.size());
	std::vector<std::atomic<int>> progress(threadPortRanges.size());

	int totalPorts = endPort - startPort + 1;
	std::optional<std::thread> monitor;
	std::atomic<bool> scanComplete(false);

	setupProgressMonitoring(monitor, progress, scanComplete, totalPorts);

	launchScanThreads(threadPortRanges, allResult, progress);
	scanComplete = true;

	if(monitor.has_value()) {
		monitor->join();
	}

	std::vector<PortResult> finalResult = aggregateResults(allResult);
	return finalResult;
}

ConnectionData PortScanner::setupConnection(int port) {

	struct ConnectionData data;

	data.socketFd = createRawSocket();
	data.sockaddr = setupSocket(_targetIp, port);
	if(!data.sockaddr) {
		cleanupConnectionData(data);
        throw std::runtime_error("Failed to setup socket: ");
	}
	// TODO: Add socket options for better performance (SO_REUSEADDR, etc.)
	return data;
}

PortStatus PortScanner::testSinglePort(int port) {

	try
	{
		ConnectionData data = setupConnection(port);

		char buffer[40];
		uint16_t srcPort = rand() % (65535 - 49152 + 1) + 49152;
		struct iphdr* ip = (struct iphdr*)buffer;
		struct tcphdr* tcp = (struct tcphdr*)(buffer + sizeof(struct iphdr));

		fillIpHeader(ip, _localIp, data.sockaddr->sin_addr.s_addr, sizeof(buffer));
		fillTcpHeader(tcp, srcPort, port);
		tcpChecksum(ip, tcp, sizeof(struct tcphdr));

		int packetLen = 40;
		int sendSocket = sendto(data.socketFd, buffer, packetLen, 0,(struct sockaddr *)data.sockaddr, sizeof(*data.sockaddr));
		if(sendSocket < 0) {
			cleanupConnectionData(data);
			return NETWORK_ERROR;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what();
		std::cerr << port << std::endl;
	}
	return PORT_CLOSED;
}

void PortScanner::scanPortRange(int start, int end, std::vector<PortResult>& results, std::atomic<int>& progress) {

	// TODO: Implement adaptive timing based on network conditions

    results.reserve(end - start + 1);
    for(int port = start; port <= end; port++) {
        PortResult res;
        res.port = port;
        res.status = testSinglePort(port);
		// TODO: Add rate limiting to avoid detection
		// TODO: Add randomization of port scan order
        results.push_back(res);
        progress++;
    }
}
