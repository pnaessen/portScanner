/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cli.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 17:17:44 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/25 17:22:48 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "cli.hpp"

void printUsageHelpers(char **argv) {

	std::cout << "Usage: " << argv[0] << " [TARGET IP] <START_PORT> <END_PORT> \n"
				  << "Example: " << argv[0] << " 192.168.1.1 20 80" << std::endl;
}