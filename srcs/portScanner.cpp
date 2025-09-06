/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portScanner.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 08:02:22 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/06 15:31:14 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

PortScanner::PortScanner(const std::string& target) {
	
	try {
		_targetIp = checkIpValid(target);
		_timeoutMs = DEFAULT_TIMEOUT_MS;
    	_threadCount = std::thread::hardware_concurrency();
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
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
			throw std::out_of_range("DNS resolution failed :") ;
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
	
	data.socketFd = socket(AF_INET,SOCK_STREAM, 0);
	data.socketPoll =  setupPoll(data.socketFd);
	data.sockaddr = setupSocket(_targetIp, port);
	if(!data.sockaddr) {
		cleanupConnectionData(data);
        throw std::runtime_error("Failed to setup socket: ");
	}
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

	
	int pollStatus = poll(data.socketPoll, 1, this->_timeoutMs);
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


std::vector<PortResult> PortScanner::scanRange(int startPort, int endPort) {
	
	auto threadPortRanges = calculateThreadDistribution(startPort, endPort);
	
	std::vector<std::future<void>> futures;
	std::vector<std::vector<PortResult>> allResult(threadPortRanges.size());
	
	for (size_t i = 0; i < threadPortRanges.size(); ++i) {
		futures.emplace_back(std::async(std::launch::async, &PortScanner::scanPortRange, this, threadPortRanges[i].first, threadPortRanges[i].second, std::ref(allResult[i])));
	}
	
	for (auto& fut : futures) {
		fut.get();
	}
			
	std::vector<PortResult> finalResult;
	for (const auto& threadResult : allResult) {
		finalResult.insert(finalResult.end(), threadResult.begin(), 	threadResult.end());
	}
	return finalResult;
}


PortStatus PortScanner::testSinglePort(int port) {

	try
	{
		ConnectionData data = setupConnection(port);
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
	catch(const std::exception& e)
	{
		std::cerr << e.what();
		std::cerr << port << std::endl;
	}
	
}

void PortScanner::scanPortRange(int start, int end, std::vector<PortResult>& results){
	
	results.reserve(end - start + 1); 
	for(int port = start; port <= end; port++) {
		PortResult res;
		res.port = port;
		res.status = testSinglePort(port);
		results.push_back(res);
	}
}
