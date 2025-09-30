/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portScanner.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 08:02:22 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/30 10:31:09 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"
#include "cli.hpp"

PortScanner::PortScanner(const std::string& target, bool flag) : _timeoutMs(DEFAULT_TIMEOUT_MS), _noProgress(flag) {

	try {
		_targetIp = checkIpValid(target);
    	_threadCount = std::thread::hardware_concurrency();
		if(getuid() != 0) {
			throw std::out_of_range("No root privilege ");
		}
		// TODO: Add option to configure timeout
	}
	catch (std::exception &e) {
		std::cerr << e.what();
		throw std::out_of_range(target) ;
	}
}

std::string PortScanner::checkIpValid(const std::string& ip) {

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
			throw std::out_of_range("DNS resolution failed: ") ;
		}
		else {
			return ip;
		}
	}
}

pollfd* PortScanner::setupPoll(int socketFd) {

	pollfd* socketPoll = new pollfd;

	socketPoll->fd = socketFd;
	socketPoll->events = POLLOUT;

	return socketPoll;
}


sockaddr_in* PortScanner::setupSocket(const std::string& ip, int port) {

	sockaddr_in* sockaddr = new sockaddr_in;


	sockaddr->sin_port = htons(port);
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_addr.s_addr = inet_addr(ip.c_str());
	return sockaddr;
}

ConnectionData PortScanner::setupConnection(int port) {

	struct ConnectionData data;

	data.socketFd = createRawSocket();
	//data.socketPoll =  setupPoll(data.socketFd);
	data.sockaddr = setupSocket(_targetIp, port);
	if(!data.sockaddr) {
		cleanupConnectionData(data);
        throw std::runtime_error("Failed to setup socket: ");
	}
	// TODO: Add socket options for better performance (SO_REUSEADDR, etc.)
	fcntl(data.socketFd, F_SETFL, O_NONBLOCK);
	return data;
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



std::vector<std::pair<int, int>> PortScanner::calculateThreadDistribution(int start, int end) {

		int totalPort =  (end - start) + 1;
		int portPerThread = totalPort / _threadCount;
		int remainder = totalPort % _threadCount ;
		int currentPort = start;
		std::vector<std::pair<int, int>> threadPortRanges;

		for (int i = 0; i < _threadCount; ++i) {
			int startPortThread = currentPort;
			int count = portPerThread + (i < remainder ? 1 : 0);
			int endPortThread = startPortThread + count - 1;
			threadPortRanges.push_back(std::make_pair(startPortThread, endPortThread));
			currentPort = endPortThread + 1;
		}
		return threadPortRanges;
}

void PortScanner::monitorProgress(std::atomic<bool>* scanComplete, std::vector<std::atomic<int>>* progress, int totalPorts, std::chrono::steady_clock::time_point startTime) {

    while (!scanComplete->load()) {

        int scannedPorts = 0;
        for (auto& p : *progress) {
			 scannedPorts += p.load();
		}
        int percent = (scannedPorts * 100) / totalPorts;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        std::cout << "\rProgress: " << percent << "% (" << scannedPorts << "/" << totalPorts << ")"
                  << " | Elapsed time: " << elapsed << "s" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(PROGRESS_UPDATE_INTERVAL_MS));
    }
    std::cout << std::endl;
}

std::vector<PortResult> PortScanner::aggregateResults(std::vector<std::vector<PortResult>>& allResult) {

	std::vector<PortResult> finalResult;
	for (const auto& threadResult : allResult) {
		finalResult.insert(finalResult.end(), threadResult.begin(), threadResult.end());
	}
	return finalResult;
}

void PortScanner::launchScanThreads(std::vector<std::pair<int, int>>& threadPortRanges, std::vector<std::vector<PortResult>>& allResult, std::vector<std::atomic<int>>& progress) {

	std::vector<std::future<void>> futures;

	for (size_t i = 0; i < threadPortRanges.size(); ++i) {
		futures.emplace_back(std::async(std::launch::async, &PortScanner::scanPortRange, this,
			threadPortRanges[i].first, threadPortRanges[i].second,
			std::ref(allResult[i]), std::ref(progress[i])));
	}

	for (auto& fut : futures) {
		fut.get();
	}
}

void PortScanner::setupProgressMonitoring(std::optional<std::thread>& monitor, std::vector<std::atomic<int>>& progress, std::atomic<bool>& scanComplete, int totalPorts) {

	auto startTime = std::chrono::steady_clock::now();

	if(_noProgress == false) {
    	monitor.emplace(&PortScanner::monitorProgress, this, &scanComplete, &progress, totalPorts, startTime);
	}
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

PortStatus PortScanner::testSinglePort(int port) {

	try
	{
		ConnectionData data = setupConnection(port);
		// TODO: Creat ip and tcp headers
		char buffer[4096];
		int packetLen = 0; // total length of  packet 40 bytes ??
		int sendSocket = sendto(data.socketFd, buffer, packetLen, 0,(struct sockaddr *)data.sockaddr, sizeof(*data.sockaddr));
		if(sendSocket < 0) {
			cleanupConnectionData(data);
			return PORT_CLOSED;
		}
		// int status = connect(data.socketFd, (struct sockaddr *)data.sockaddr, sizeof(*data.sockaddr));
		// if(status < 0 && errno == EINPROGRESS) {
		// 	PortStatus result = handleAsyncConnect(data);
		// 	cleanupConnectionData(data);
		// 	return result;
		// }
		// else if(status == 0) {
		// 	cleanupConnectionData(data);
		// 	return PORT_OPEN;
		// }
		// cleanupConnectionData(data);
		// return PORT_FILTERED;
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


int PortScanner::createRawSocket() {

	int socketFd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	int error = 0;
	if(socketFd < 0) {
		throw std::runtime_error("Fail socket creation");
	}

	setsockopt(socketFd, IPPROTO_IP, IP_HDRINCL, &error, sizeof(error));
	if(error < 0) {
		throw std::runtime_error("Fail settings socket");
	}
    return socketFd;
}