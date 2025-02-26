#include <print>
#include <thread>
#include <vector>
#include <atomic>
#include <stop_token>

using std::chrono_literals::operator""s;

auto heavyComputation(std::stop_token stopToken)
{
    const auto stopCallback = std::stop_callback{stopToken, []
    {
        std::print("Stopping thread: {}\n", std::this_thread::get_id()); 
    }};

    while (not stopToken.stop_requested())
    {
        // compute until cancel detected
        std::print("Processing in progress. Thread id: {}\n", std::this_thread::get_id()); 
        std::this_thread::sleep_for(1s);
    }   
}

auto doWork(int maxThreads)
{
    auto stopSource = std::stop_source{};
    auto jthreads = std::vector<std::jthread>();
    for (int i = 0; i < maxThreads; i++)
    {
        jthreads.emplace_back(heavyComputation, stopSource.get_token());
    }

    std::this_thread::sleep_for(5s); // simulate some work
    stopSource.request_stop(); // stop all threads simultaneously
}

int main()
{
    std::print("Main thread id: {}\n", std::this_thread::get_id());
    doWork(std::thread::hardware_concurrency());
    return 0;
}
// Listing 12: Stop callback moved inside jthread