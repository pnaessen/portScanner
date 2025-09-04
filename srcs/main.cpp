/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pn <pn@student.42lyon.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/04 20:32:09 by pn               ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

std::vector<std::pair<int, int>> setupThreadDistribution(int startPort, int endPort) {
	
		int nbThread = std::thread::hardware_concurrency();
	//	std::cout << "nb de thread: " << nbThread << std::endl; 
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
			//  std::cout << "Thread " << i << ": ports " << startPortThread 
			//   << " to " << endPortThread << " (" << count << " ports)" << std::endl;
		}
		return threadPortRanges;
}

int main() {

	try {
		std::string ip = checkIpValid("google.com");

		
		auto threadPortRanges  = setupThreadDistribution(DEFAULT_START_PORT, DEFAULT_END_PORT );
		
		std::vector<std::future<void>> futures;
		std::vector<std::vector<PortResult>> allResult(threadPortRanges.size());

		for (size_t i = 0; i < threadPortRanges.size(); ++i) {
			futures.emplace_back(std::async(std::launch::async, workerThread, ip, threadPortRanges[i].first, threadPortRanges[i].second, std::ref(allResult[i])));
			}
			
			for (auto& fut : futures) {
				fut.get();
			}
			
			std::vector<PortResult> finalResult;
			for (const auto& threadResult : allResult) {
				finalResult.insert(finalResult.end(), threadResult.begin(), 	threadResult.end());
			}
			
			for(size_t i = 0; i < finalResult.size(); i++) {			
			auto result = finalResult[i];
			std::cout << "Port " << result.port << ": " << result.status << std::endl;
			}

	}
	catch (const std::exception &e) {
		return 1;
	}
    return 0;
}



