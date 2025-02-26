#include <print>
#include <thread>
#include <vector>

auto prepareForResults()
{
    std::print("Prepare before processing is done. Thread id: {}\n", std::this_thread::get_id());
}

auto heavyComputation(int i)
{
    std::print("Processing in progress. Thread id: {}\n", std::this_thread::get_id()); 
    std::this_thread::sleep_for(std::chrono::seconds(i + 5));
}

auto doWork(int maxThreads)
{
    auto threads = std::vector<std::thread>();
    for (int i = 0; i < maxThreads - 1; i++)
    {
        threads.emplace_back(heavyComputation, i);
    }

    prepareForResults();

    for (auto& thread : threads)
    {
        std::print("Joining thread: {}\n", thread.get_id());
        thread.join(); // join every thread
    }
}

int main()
{
    doWork(std::thread::hardware_concurrency());
    return 0;
}
// Listing 4: Starting mutliple threads