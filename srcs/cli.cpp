/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cli.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 17:17:44 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/26 13:50:11 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "cli.hpp"

void printUsageHelpers(char **argv) {

	std::cout << "Usage: " << argv[0] << " [TARGET IP] <START_PORT> <END_PORT> \n"
				  << "Example: " << argv[0] << " 192.168.1.1 20 80" << std::endl;
}

int countPositionalArgs(int argc, char ** argv) {

	int posCount = 0;
	for(int i = 0; i < argc; i++) {
		if(argv[i][0] != '-') {
			posCount++;
		}
	}
	return posCount;
}

ParseStatus checkFlagValidity(char *str, ScanConfig& flag) {

	for(int i = 0; str[i]; i++) {

		if(str[0] == '-' && str[1] && i < 1) {
			if(str[1] == 'a') {
				flag.showAllPorts = true;
			}
			else if (str[1] == 'h') {
				flag.helpRequested = true;
			}
			else
				return PARSE_ERROR;
		}
		if(i > 2) {
			return PARSE_ERROR;
		}
	}
	return PARSE_OK;
}

ParseStatus parseArgs(int argc, char **argv, int& startPort, int& endPort, ScanConfig& flag) {

	int posCount = countPositionalArgs(argc, argv);

	if (posCount < 2 || argc > 5) {
		printUsageHelpers(argv);
		return PARSE_INVALID_ARG_COUNT;
	}
	if(posCount > 2) {
		std::stringstream ss(argv[2]);
		if (!(ss >> startPort) || !ss.eof()) {
			printUsageHelpers(argv);
			return PARSE_ERROR;
		}
	}
	if(posCount > 3) {
		std::stringstream ss(argv[3]);
		if (!(ss >> endPort) || !ss.eof()) {
			printUsageHelpers(argv);
			return PARSE_ERROR;
		}
	}
	if(posCount != argc) {
		if(checkFlagValidity(argv[argc - 1], flag) != PARSE_OK)
			return PARSE_ERROR;
	}

	if(checkPortsValidity(startPort, endPort) != PARSE_OK) {
			return PARSE_INVALID_PORT;
	}
	return PARSE_OK;
}

ParseStatus checkPortsValidity(int startPort, int endPort) {

	if(startPort < DEFAULT_START_PORT || startPort > DEFAULT_END_PORT) {
		std::cout << "Usage: <START_PORT> must be between "
				  << DEFAULT_START_PORT << " and " << DEFAULT_END_PORT << std::endl;
		return PARSE_INVALID_PORT;
		}

	if(endPort < startPort || endPort > DEFAULT_END_PORT) {
			std::cout << "Usage: <END_PORT> must be greater than or equal to <START_PORT> and less than or equal to "
                      << DEFAULT_END_PORT << std::endl;
			return PARSE_INVALID_PORT;
		}

	return PARSE_OK;
}