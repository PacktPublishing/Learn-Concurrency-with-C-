#include <print>
#include <thread>
#include <vector>
#include <atomic>
#include <stop_token>

using std::chrono_literals::operator""s;

auto heavyComputation(std::stop_token stopToken)
{
    while (not stopToken.stop_requested())
    {
        // compute until cancel detected
        std::print("Processing in progress. Thread id: {}\n", std::this_thread::get_id()); 
        std::this_thread::sleep_for(1s);
    }   
}

auto doWork(int maxThreads)
{
    auto jthreads = std::vector<std::jthread>();
    for (int i = 0; i < maxThreads; i++)
    {
        jthreads.emplace_back(heavyComputation);
    }

    std::this_thread::sleep_for(5s); // simulate some work
}

int main()
{
    doWork(std::thread::hardware_concurrency());
    return 0;
}
// Listing 8: JThread cancelation