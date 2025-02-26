#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

std::atomic_flag cancel_flag{};

auto heavyComputation()
{
    std::cout << "this function is processing data\n";
    while (cancel_flag.test() == false)
    {
        // compute until cancel detected
        std::cout << "computing...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }   
}

auto doWork(int maxThreads)
{
    auto threads = std::vector<std::thread>();
    for (int i = 0; i < maxThreads; i++)
    {
        threads.push_back(std::thread(heavyComputation));
    }

    try
    {
        std::this_thread::sleep_for(std::chrono::seconds(5)); // simulate some work
        cancel_flag.test_and_set(); // set atomic cancel flag
    }
    catch (const std::exception& e)
    {
        std::cerr << "exception caught: " << e.what() << '\n';
        for (auto& thread : threads)
        {
            thread.join(); // join every thread before rethrowing the exception
        }
        throw; // let the exception propagate to handle it elsewhere
    }

    for (auto& thread : threads)
    {
        thread.join(); // join every thread
    }
}

int main()
{
    doWork(3);
    return 0;
}