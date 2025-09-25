/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cli.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 17:19:19 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/25 17:41:44 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLI_HPP
# define CLI_HPP

#include <iostream>
#include <string>
#include <sstream>
#include "portScanner.hpp"

enum ParseStatus {
	PARSE_OK,
	PARSE_INVALID_ARG_COUNT,
    PARSE_INVALID_PORT,
	PARSE_ERROR
};

void printUsageHelpers(char **argv);
ParseStatus checkArgs(int argc, char **argv, int& startPort, int& endPort);

#endif /* CLI_HPP */