/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threadUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 11:06:36 by pnaessen          #+#    #+#             */
/*   Updated: 2025/10/09 11:09:17 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include  "portScanner.hpp"

std::vector<std::pair<int, int>> PortScanner::calculateThreadDistribution(int start, int end) {

		int totalPort =  (end - start) + 1;
		int portPerThread = totalPort / _threadCount;
		int remainder = totalPort % _threadCount ;
		int currentPort = start;
		std::vector<std::pair<int, int>> threadPortRanges;

		for (int i = 0; i < _threadCount; ++i) {
			int startPortThread = currentPort;
			int count = portPerThread + (i < remainder ? 1 : 0);
			int endPortThread = startPortThread + count - 1;
			threadPortRanges.push_back(std::make_pair(startPortThread, endPortThread));
			currentPort = endPortThread + 1;
		}
		return threadPortRanges;
}

void PortScanner::launchScanThreads(std::vector<std::pair<int, int>>& threadPortRanges, std::vector<std::vector<PortResult>>& allResult, std::vector<std::atomic<int>>& progress) {

	std::vector<std::future<void>> futures;

	for (size_t i = 0; i < threadPortRanges.size(); ++i) {
		futures.emplace_back(std::async(std::launch::async, &PortScanner::scanPortRange, this,
			threadPortRanges[i].first, threadPortRanges[i].second,
			std::ref(allResult[i]), std::ref(progress[i])));
	}

	for (auto& fut : futures) {
		fut.get();
	}
}

std::vector<PortResult> PortScanner::aggregateResults(std::vector<std::vector<PortResult>>& allResult) {

	std::vector<PortResult> finalResult;
	for (const auto& threadResult : allResult) {
		finalResult.insert(finalResult.end(), threadResult.begin(), threadResult.end());
	}
	return finalResult;
}
