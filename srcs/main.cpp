/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pn <pn@student.42lyon.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/03 21:43:17 by pn               ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

std::vector<std::pair<int, int>> setupThreadDistribution(int startPort, int endPort) {
	
		int nbThread = std::thread::hardware_concurrency();
		int totalPort =  (endPort - startPort) + 1;
		int portPerThread = totalPort / nbThread;
		int remainder = totalPort % nbThread ;
		int currentPort = startPort;
		std::vector<std::pair<int, int>> threadPortRanges;
		
		for (int i = 0; i < nbThread; ++i) {
			int startPortThread = currentPort;
			int count = portPerThread + (i < remainder ? 1 : 0);
			int endPortThread = startPortThread + count - 1;
			threadPortRanges.push_back(std::make_pair(startPortThread, endPortThread));
			currentPort = endPortThread + 1;
			 std::cout << "Thread " << i << ": ports " << startPortThread 
			  << " to " << endPortThread << " (" << count << " ports)" << std::endl;
		}
		return threadPortRanges;
}

int main() {

	try {
		
		std::vector<int> ports = {80, 443, 22, 8, 25, 66, 44, 333, 21, 8, 56, 31};
		std::vector<std::future<PortResult>> futures;
		std::string ip = checkInput("google.com");

		setupThreadDistribution(1, 1000);
		return 0;
		
		for (int port : ports) {
			auto future = std::async(std::launch::async, testPortAsync, ip, port);
			futures.push_back(std::move(future));
		}

		for (auto& fut : futures) {
			PortResult result = fut.get();
			std::cout << "Port " << result.port << ": " << result.status << std::endl;
		}
	}
	catch (const std::exception &e) {
		return 1;
	}
    return 0;
}



