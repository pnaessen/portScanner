#include "cli.hpp"


std::vector<PortResult> PortScanner::scanRange(int startPort, int endPort) {

    auto threadPortRanges = calculateThreadDistribution(startPort, endPort);

    std::vector<std::future<void>> futures;
    std::vector<std::vector<PortResult>> allResult(threadPortRanges.size());
    std::vector<std::atomic<int>> progress(threadPortRanges.size());

    int totalPorts = endPort - startPort + 1;
    auto startTime = std::chrono::steady_clock::now();

    std::atomic<bool> done(false);
    std::thread monitor([&](){
        while (!done) {
            int checked = 0;
            for (auto& p : progress) checked += p.load();
            int percent = (checked * 100) / totalPorts;
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
            std::cout << "\rProgress: " << percent << "% (" << checked << "/" << totalPorts << ")"
                << " | Elapsed time: " << elapsed << "s" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        std::cout << std::endl;
    });

    for (size_t i = 0; i < threadPortRanges.size(); ++i) {
        futures.emplace_back(std::async(std::launch::async, [&, i](){
            for (int port = threadPortRanges[i].first; port <= threadPortRanges[i].second; ++port) {
                PortResult res;
                res.port = port;
                res.status = testSinglePort(port);
                allResult[i].push_back(res);
                progress[i]++;
            }
        }));
    }

    for (auto& fut : futures) fut.get();
    done = true;
    monitor.join();

	
    std::vector<PortResult> finalResult;

    for (const auto& threadResult : allResult)
        finalResult.insert(finalResult.end(), threadResult.begin(), threadResult.end());

    return finalResult;
}