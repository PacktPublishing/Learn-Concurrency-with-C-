#include <print>
#include <thread>
#include <future>
#include <vector>
#include <chrono>

using std::chrono_literals::operator""s;

auto heavyComputation(int i)
{
    std::this_thread::sleep_for(3s);
    return i + 10;
}

auto doWork(int maxThreads) 
{
    std::print("Starting computation...\n");
    auto resultFutures = std::vector<std::future<int>>();
    for (int i = 0; i < maxThreads; i++)
    {
        // launch a thread deferred, store future in a vector
        resultFutures.emplace_back(std::async(std::launch::deferred, heavyComputation, i));
    }

    std::print("Waiting for the results...\n");
    for (auto& f : resultFutures)
    {
        try
        {
            // print that the thread is not launched yet
            std::print("Future status deferred: {}\n", f.wait_for(0s) == std::future_status::deferred); 

            // launch the thread with .get() call, block until ready, print out the result
            std::print("Result: {}\n", f.get()); 
        }
        catch (const std::exception& e)
        {
            std::print("Caught exception: {}\n", e.what()); // catch any exceptions of they happen
        }
    }
}

int main()
{
    doWork(std::thread::hardware_concurrency());
    return 0;
}
// Listing 18: Launching threads and passing future/promise values with deferred async