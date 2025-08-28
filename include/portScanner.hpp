/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portScanner.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:57:39 by pnaessen          #+#    #+#             */
/*   Updated: 2025/08/27 07:58:11 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PORTSCANNER_HPP
# define PORTSCANNER_HPP

	#include <string>
	#include <iostream>
	#include <poll.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
    #include <netdb.h>


	enum PortStatus {
		PORT_OPEN,
		PORT_CLOSED,
		PORT_FILTERED
	};

	struct ConnectionData {
   	 	pollfd* socketPoll;
   		sockaddr_in* sockaddr;
   		int socketFd;
	};

	PortStatus test_port(const std::string& ip, const int port);

#endif