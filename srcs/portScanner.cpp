/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portScanner.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 08:02:22 by pnaessen          #+#    #+#             */
/*   Updated: 2025/10/03 11:38:59 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"
#include "cli.hpp"

PortScanner::PortScanner(const std::string& targetIP, bool flag) : _timeoutMs(DEFAULT_TIMEOUT_MS), _noProgress(flag) {

	try {
		_targetIp = checkIpValid(targetIP);
    	_threadCount = std::thread::hardware_concurrency();
		_localIp = getLocalIP("8.8.8.8");
		// if(getuid() != 0) {
		// 	throw std::out_of_range("No root privilege ");
		// }
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
	data.sockaddr = setupSocket(_targetIp, port);
	if(!data.sockaddr) {
		cleanupConnectionData(data);
        throw std::runtime_error("Failed to setup socket: ");
	}
	// TODO: Add socket options for better performance (SO_REUSEADDR, etc.)
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

uint16_t ipChecksum(void* hdr, int len) {

    uint32_t sum = 0;
    uint16_t* ptr = (uint16_t*)hdr;

    for (int i = 0; i < len / 2; i++) {
        sum += ptr[i];
        if (sum > 0xFFFF) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
    }

	if (len % 2) {
        sum += ((uint8_t*)hdr)[len - 1] << 8;
        if (sum > 0xFFFF) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
    }

    return htons(~sum & 0xFFFF);
}

void PortScanner::tcpChecksum(struct iphdr* ip, struct tcphdr* tcp, int len) {

	struct pseudoHeader pseudoHdr;
	pseudoHdr.src_addr = ip->saddr;
	pseudoHdr.dst_addr = ip->daddr;
	pseudoHdr.zero = 0;
	pseudoHdr.protocol = IPPROTO_TCP;
	pseudoHdr.tcp_length = htons(len);
	tcp->syn = 1;

	char bufferTmp[sizeof(pseudoHeader) + len];
	std::memcpy(bufferTmp, &pseudoHdr, sizeof(struct pseudoHeader));
	std::memcpy(bufferTmp + sizeof(struct pseudoHeader), tcp, len);

	tcp->check = ipChecksum(bufferTmp, sizeof(struct pseudoHeader) + len);
}

void PortScanner::fillIpHeader(struct iphdr* ip, const std::string& srcIp, in_addr_t& dstIp, int totalLen) {

	ip->version = 4;
	ip->ihl = 5;
	ip->tos = 0;
	ip->tot_len = htons(totalLen);
	ip->id = htons(54321);
	ip->frag_off = 0;
	ip->ttl = 64;
	ip->protocol = IPPROTO_TCP;
	ip->saddr = inet_addr(srcIp.c_str());
	ip->daddr = dstIp;
	ip->check = 0;
	ip->check = ipChecksum(ip, sizeof(struct iphdr));

}

void PortScanner::fillTcpHeader(struct tcphdr* tcp, uint16_t srcPort, int port) {

	tcp->source = htons(srcPort);
	tcp->dest = htons(port);
	tcp->seq = htonl(0);
	tcp->ack_seq = 0;
	tcp->doff = 5;
	tcp->syn = 1;
	tcp->window = htons(5840);
	tcp->urg_ptr = 0;
	tcp->check = 0;
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