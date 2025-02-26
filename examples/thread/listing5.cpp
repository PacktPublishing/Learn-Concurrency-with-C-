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
    auto threads = std::vector<std::thread>();
    for (int i = 0; i < maxThreads - 1; i++)
    {
        threads.emplace_back(heavyComputation);
    }

    try
    {
        prepareForResults();
    }
    catch (const std::exception& e)
    {
        for (auto& t : threads)
        {
            t.join(); // join every thread before rethrowing the exception
        }
        throw; // let the exception propagate to handle it elsewhere
    }

    for (auto& t : threads)
    {
        t.join(); // join every thread
    }
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
// Listing 5: DoWork function with try-catch block