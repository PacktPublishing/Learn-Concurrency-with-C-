#include <print>
#include <thread>
#include <vector>

auto prepareForResults()
{
    std::print("Prepare before processing is done. Thread id: {}\n", std::this_thread::get_id());
}

auto heavyComputation()
{
    std::print("Processing in progress. Thread id: {}\n", std::this_thread::get_id()); 
}

auto doWork(int maxThreads)
{
    auto jthreads = std::vector<std::jthread>();
    for (int i = 0; i < maxThreads - 1; i++)
    {
        jthreads.emplace_back(heavyComputation);
    }
    prepareForResults();
}

int main()
{
    try 
    {
        doWork(std::thread::hardware_concurrency());
    }
    catch (const std::exception& e)
    {
        std::print("Caught exception: {}\n", e.what());
    }
    return 0;
}
// Listing 6: DoWork function with jthread class