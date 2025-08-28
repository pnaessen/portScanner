/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/08/28 09:25:28 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

int main() {
	
    std::cout << "Test port 53: " << test_port("8.8.8.8", 53) << std::endl;
    std::cout << "Test port 12345: " << test_port("8.8.8.8", 12345) << std::endl;
	std::cout << "Port 80 (HTTP): " << test_port("8.8.8.8", 80) << std::endl;
	std::cout << "Port 443 (HTTPS): " << test_port("8.8.8.8", 443) << std::endl;
	std::cout << "Port 22 (SSH): " << test_port("8.8.8.8", 22) << std::endl;
	return 0;
}