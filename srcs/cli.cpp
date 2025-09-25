/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cli.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 17:17:44 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/25 17:42:23 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "cli.hpp"

void printUsageHelpers(char **argv) {

	std::cout << "Usage: " << argv[0] << " [TARGET IP] <START_PORT> <END_PORT> \n"
				  << "Example: " << argv[0] << " 192.168.1.1 20 80" << std::endl;
}

ParseStatus checkArgs(int argc, char **argv, int& startPort, int& endPort) {

	if (argc < 2 || argc > 4) {
		printUsageHelpers(argv);
		return PARSE_INVALID_ARG_COUNT;
	}
	if(argc > 2) {
		std::stringstream ss(argv[2]);
		if (!(ss >> startPort) || !ss.eof()) {
			printUsageHelpers(argv);
			return PARSE_ERROR;
		}
		if(startPort < DEFAULT_START_PORT || startPort > DEFAULT_END_PORT) {
			std::cout << "Usage: <START_PORT> must be between "
					  << DEFAULT_START_PORT << " and " << DEFAULT_END_PORT << std::endl;
			return PARSE_INVALID_PORT;
		}
	}
	if(argc > 3) {
		std::stringstream ss(argv[3]);
		if (!(ss >> endPort) || !ss.eof()) {
			printUsageHelpers(argv);
			return PARSE_ERROR;
		}
		if(endPort < startPort || endPort > DEFAULT_END_PORT) {
			std::cout << "Usage: <END_PORT> must be greater than or equal to <START_PORT> and less than or equal to "
                      << DEFAULT_END_PORT << std::endl;
			return PARSE_INVALID_PORT;
		}
	}
	return PARSE_OK;
}