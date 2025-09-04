/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pn <pn@student.42lyon.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/04 12:01:04 by pn               ###   ########lyon.fr   */
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
		
		// std::vector<int> ports = {80, 443, 22, 8, 25, 66, 44, 333, 21, 8, 56, 31};
		// std::vector<std::future<PortResult>> futures;
		std::string ip = checkInput("google.com");

		
		auto ranges = setupThreadDistribution(1, 50);
		std::vector<std::future<void>> futures;
		std::vector<std::vector<PortResult>> all_results(ranges.size());

		for (size_t i = 0; i < ranges.size(); ++i) {
			futures.emplace_back(worker_thread, ip, ranges[i].first, ranges[i].second,std::ref(all_results[i]));
		}

		int i = 0;
		for (auto& fut : futures) {
   			fut.get();
			auto result = all_results[i];
			std::cout << "Port " << result[i].port << ": " << result[i].status << std::endl;
			i++;
		}
		// for (int port : ports) {
		// 	auto future = std::async(std::launch::async, testPortAsync, ip, port);
		// 	futures.push_back(std::move(future));
		// }

		// for (auto& fut : futures) {
		// 	PortResult result = fut.get();
		// 	std::cout << "Port " << result.port << ": " << result.status << std::endl;
		// }
	}
	catch (const std::exception &e) {
		return 1;
	}
    return 0;
}



