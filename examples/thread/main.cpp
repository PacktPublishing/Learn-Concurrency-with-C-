#include <iostream>
#include <thread>
#include <vector>

auto prepareForResults()
{
    std::cout << "here we'll prepare for the results while processing is in progress\n";
}

auto heavyComputation()
{
    std::cout << "this function is processing data\n";
    for (int i = 0; i < 1000000; i++)
    {
        // perform i-th iteration of computation
    }   
}

auto doWork(int maxThreads)
{
    auto threads = std::vector<std::thread>();
    for (int i = 0; i < maxThreads - 1; i++)
    {
        threads.push_back(std::thread(heavyComputation));
    }

    try
    {
        prepareForResults();
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
    doWork(std::thread::hardware_concurrency());
    return 0;
}