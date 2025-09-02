/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/02 13:43:19 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

int main() {

	try {
		
		std::vector<int> ports = {80, 443, 22, 8, 25};
		std::vector<std::future<PortResult>> futures;
		std::string ip = checkInput("google.com");

		
		for (int port : ports) {
			auto future = std::async(std::launch::async, testPortAsync, ip, port);
			futures.push_back(std::move(future));
		}
	}
	catch (const std::exception &e) {
		return 1;
	}
    return 0;
}

