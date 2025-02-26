#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <stop_token>

using std::chrono_literals::operator""s;

auto heavyComputation(std::stop_token stopToken)
{
    std::cout << "this function is processing data\n";
    while (not stopToken.stop_requested())
    {
        // compute until cancel detected
        std::cout << "computing...\n";
        std::this_thread::sleep_for(1s);
    }
    std::cout << "processing finished\n";
}

auto doWork(int maxThreads)
{
    std::stop_source stop_source;
    auto jthreads = std::vector<std::jthread>();
    for (int i = 0; i < maxThreads; i++)
    {
        jthreads.emplace_back(heavyComputation, stop_source.get_token());
    }

    std::this_thread::sleep_for(5s); // simulate some work
    stop_source.request_stop();
}

int main()
{
    doWork(2);
    return 0;
}