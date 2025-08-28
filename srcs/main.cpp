/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/08/28 14:03:12 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

int main() {
	
    std::cout << "Test port 53: " << test_port("google.com", 53) << std::endl;
    std::cout << "Test port 12345: " << test_port("google.com", 12345) << std::endl;
	std::cout << "Port 80 (HTTP): " << test_port("google.com", 80) << std::endl;
	std::cout << "Port 443 (HTTPS): " << test_port("8.8.8.8", 443) << std::endl;
	std::cout << "Port 22 (SSH): " << test_port("google.com", 22) << std::endl;
	std::cout << "Facebook: " << test_port("facebook.com", 80) << std::endl;
	std::cout << "GitHub SSH: " << test_port("github.com", 22) << std::endl;
	std::cout << "BAD DOMAIN SSH: " << test_port("dfhakfgdasfhdsgfghsdjckhjkshckshcshk sdcgsdchsd s.com", 22) << std::endl;
	
	return 0;
}

