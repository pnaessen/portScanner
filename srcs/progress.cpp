/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   progress.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnaessen <pnaessen@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 11:08:30 by pnaessen          #+#    #+#             */
/*   Updated: 2025/10/09 11:09:07 by pnaessen         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "portScanner.hpp"

void PortScanner::setupProgressMonitoring(std::optional<std::thread>& monitor, std::vector<std::atomic<int>>& progress, std::atomic<bool>& scanComplete, int totalPorts) {

	auto startTime = std::chrono::steady_clock::now();

	if(_noProgress == false) {
    	monitor.emplace(&PortScanner::monitorProgress, this, &scanComplete, &progress, totalPorts, startTime);
	}
}

void PortScanner::monitorProgress(std::atomic<bool>* scanComplete, std::vector<std::atomic<int>>* progress, int totalPorts, std::chrono::steady_clock::time_point startTime) {

    while (!scanComplete->load()) {

        int scannedPorts = 0;
        for (auto& p : *progress) {
			 scannedPorts += p.load();
		}
        int percent = (scannedPorts * 100) / totalPorts;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        std::cout << "\rProgress: " << percent << "% (" << scannedPorts << "/" << totalPorts << ")"
                  << " | Elapsed time: " << elapsed << "s" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(PROGRESS_UPDATE_INTERVAL_MS));
    }
    std::cout << std::endl;
}

