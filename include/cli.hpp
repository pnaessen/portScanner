/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cli.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 17:19:19 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/28 11:15:33 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLI_HPP
# define CLI_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include "portScanner.hpp"

struct ScanConfig {
    bool showAllPorts = false;
    bool helpRequested = false;
	bool mute = false;
};

enum ParseStatus {
	PARSE_OK,
	PARSE_INVALID_ARG_COUNT,
    PARSE_INVALID_PORT,
	PARSE_ERROR
};

void printUsageHelpers(char **argv);
ParseStatus parseArgs(int argc, char **argv, int& startPort, int& endPort, ScanConfig& flag);
int countPositionalArgs(int argc, char **argv);
ParseStatus checkPortsValidity(int startPort, int endPort);
ParseStatus checkFlagValidity(char *str, ScanConfig& flag);
void printUsageMenu();

#endif /* CLI_HPP */