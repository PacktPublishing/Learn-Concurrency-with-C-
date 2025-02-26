#include <print>
#include <thread>
#include <vector>
#include <atomic>

using std::chrono_literals::operator""s;

std::atomic_flag cancel_flag{}; // safe storage of cancellation request

auto heavyComputation()
{
    while (not cancel_flag.test())
    {
        // compute until cancel detected
        std::print("Processing in progress. Thread id: {}\n", std::this_thread::get_id()); 
        std::this_thread::sleep_for(1s);
    }   
} 

auto doWork(int maxThreads)
{
    auto threads = std::vector<std::thread>();
    for (int i = 0; i < maxThreads; i++)
    {
        threads.emplace_back(heavyComputation);
    }

    try
    {
        std::this_thread::sleep_for(5s); // simulate some work
        cancel_flag.test_and_set(); // set atomic cancel flag
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
    doWork(std::thread::hardware_concurrency());
    return 0;
}
// Listing 7: Thread cancellation