/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/03 15:22:31 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

int main() {

	try {
		
		std::vector<int> ports = {80, 443, 22, 8, 25, 66, 44, 333, 21, 8, 56, 31};
		std::vector<std::future<PortResult>> futures;
		std::string ip = checkInput("google.com");

		
		
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



void setupThreadDistribution(size_t size) {
	
		unsigned int nbThread = std::thread::hardware_concurrency();
		int portPerThread = size / nbThread;
}


// int currentPort = 0;
// for (int i = 0; i < nbThread; ++i) {
//     int startPort = currentPort;
//     int count = portPerThread + (i < remainder ? 1 : 0);
//     int end_port = startPort + count - 1;
//     thread_port_ranges.push_back(std::make_pair(start_port, end_port));
//     currentPort = end_port + 1;
// }
