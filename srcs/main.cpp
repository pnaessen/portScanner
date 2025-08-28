/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/08/28 15:58:45 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int port = 1; port <= 10; port++) {
        PortStatus result = test_port("google.com", port);
        std::cout << "Port " << port << ": " << result << std::endl;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << duration.count() << std::endl;

    return 0;
}

