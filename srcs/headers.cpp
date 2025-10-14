/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 07:40:59 by pnaessen          #+#    #+#             */
/*   Updated: 2025/10/14 17:37:43 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"
#include <cstring>


uint16_t ipChecksum(void* hdr, int len) {

    uint32_t sum = 0;
    uint16_t* ptr = (uint16_t*)hdr;

    for (int i = 0; i < len / 2; i++) {
        sum += ptr[i];
        if (sum > 0xFFFF) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
    }

	if (len % 2) {
        sum += ((uint8_t*)hdr)[len - 1] << 8;
        if (sum > 0xFFFF) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
    }

    return htons(~sum & 0xFFFF);
}

void PortScanner::tcpChecksum(struct iphdr* ip, struct tcphdr* tcp, int len) {

	struct pseudoHeader pseudoHdr;
	pseudoHdr.src_addr = ip->saddr;
	pseudoHdr.dst_addr = ip->daddr;
	pseudoHdr.zero = 0;
	pseudoHdr.protocol = IPPROTO_TCP;
	pseudoHdr.tcp_length = htons(len);
	tcp->syn = 1;

	char bufferTmp[sizeof(pseudoHeader) + len];
	std::memcpy(bufferTmp, &pseudoHdr, sizeof(struct pseudoHeader));
	std::memcpy(bufferTmp + sizeof(struct pseudoHeader), tcp, len);

	tcp->check = ipChecksum(bufferTmp, sizeof(struct pseudoHeader) + len);
}

void PortScanner::fillIpHeader(struct iphdr* ip, const std::string& srcIp, in_addr_t& dstIp, int totalLen) {

	ip->version = 4;
	ip->ihl = 5;
	ip->tos = 0;
	ip->tot_len = htons(totalLen);
	ip->id = htons(54321);
	ip->frag_off = 0;
	ip->ttl = 64;
	ip->protocol = IPPROTO_TCP;
	ip->saddr = inet_addr(srcIp.c_str());
	ip->daddr = dstIp;
	ip->check = 0;
	ip->check = ipChecksum(ip, sizeof(struct iphdr));

}

void PortScanner::fillTcpHeader(struct tcphdr* tcp, uint16_t srcPort, int port) {

	tcp->source = htons(srcPort);
	tcp->dest = htons(port);
	tcp->seq = htonl(0);
	tcp->ack_seq = 0;
	tcp->doff = 5;
	tcp->syn = 1;
	tcp->window = htons(5840);
	tcp->urg_ptr = 0;
	tcp->check = 0;
}
