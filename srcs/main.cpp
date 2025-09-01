/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/01 10:14:04 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

int main() {

	try {
		
		std::string ip = checkInput("google.com");
		
		std::cout << "Port 80 (HTTP): " << test_port(ip, 80) << std::endl;
		std::cout << "Port 443 (HTTPS): " << test_port(ip, 443) << std::endl;
		std::cout << "Port 8 (RANDOM): " << test_port(ip, 8) << std::endl;
		std::cout << "Port 22 (SSH): " << test_port(ip, 22) << std::endl;
	}
	catch (const std::exception &e) {
		return 1;
	}
    return 0;
}

