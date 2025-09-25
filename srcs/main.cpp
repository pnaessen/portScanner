/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/09/25 17:57:51 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"
#include "cli.hpp"

// TODO: Add help menu and usage instructions
// TODO: Add verbose/quiet mode options

int main(int argc ,char **argv) {

	int startPort = DEFAULT_START_PORT;
	int endPort = DEFAULT_END_PORT;

	if(parseArgs(argc, argv, startPort, endPort) != PARSE_OK)
		return 1;

	try {

		PortScanner scanner(argv[1]);
		auto finalResult = scanner.scanRange(startPort, endPort);

		// TODO: Improve output formatting (colors, tables, etc.)
		// TODO: Add option to save results to file (JSON, XML, CSV)
		// TODO: Show only open ports by default, add flag for all
		for(size_t i = 0; i < finalResult.size(); i++) {
			auto result = finalResult[i];
			std::cout << "Port " << result.port << ": " << result.status << std::endl;
		}
	 }
	catch (const std::exception &e) {
		return 1;
	}
    return 0;
}



// struct tcp_header {
//     uint16_t source_port;      // Ex: 12345 (aléatoire)
//     uint16_t dest_port;        // 80
//     uint32_t seq_number;       // Ex: 0xABCD1234 (aléatoire)
//     uint32_t ack_number;       // 0
//     uint8_t  header_len_res;   // (5 << 4) | 0 = 0x50
//     uint8_t  flags;            // 0x02 (SYN)
//     uint16_t window_size;      // 65535
//     uint16_t checksum;         // Calculé plus tard
//     uint16_t urgent_pointer;   // 0
// };


// En-tête IP (20 bytes) - Détail bit par bit

// Offset |  Field           | Taille | Description
// -------|------------------|--------|---------------------------
// 0      | Version + IHL    | 4+4 bits | IPv4 (4) + longueur en-tête (5)
// 1      | Type of Service  | 8 bits | Priorité (généralement 0)
// 2-3    | Total Length     | 16 bits| Longueur totale paquet
// 4-5    | Identification   | 16 bits| ID unique du paquet
// 6-7    | Flags + Offset   | 3+13 bits| Fragmentation
// 8      | Time to Live     | 8 bits | TTL (ex: 64)
// 9      | Protocol         | 8 bits | TCP = 6
// 10-11  | Header Checksum  | 16 bits| Somme de contrôle IP
// 12-15  | Source IP        | 32 bits| Votre IP
// 16-19  | Destination IP   | 32 bits| IP cible

// ========== RAW SOCKET IMPLEMENTATION TODOS ==========
// TODO: CRITICAL - Check for root privileges before raw socket creation
// TODO: CRITICAL - Implement proper IP header construction
// TODO: CRITICAL - Implement TCP header construction with SYN flag
// TODO: CRITICAL - Calculate IP and TCP checksums correctly
// TODO: CRITICAL - Set up packet capture to receive SYN-ACK responses
// TODO: CRITICAL - Parse received packets to determine port status
// TODO: MEDIUM - Add support for IPv6 raw sockets
// TODO: MEDIUM - Implement packet fragmentation handling
// TODO: LOW - Add stealth scanning techniques (decoy packets)
// TODO: LOW - Implement OS fingerprinting based on responses