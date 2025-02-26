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
        // launch a thread asynchronously, store future in a vector
        resultFutures.emplace_back(std::async(std::launch::async, heavyComputation, i));
    }

    std::print("Waiting for the results...\n");
    for (auto& f : resultFutures)
    {
        try
        {
            std::print("Result: {}\n", f.get()); // block until result is available and print it
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
// Listing 17: Launching threads and passing future/promise values with async