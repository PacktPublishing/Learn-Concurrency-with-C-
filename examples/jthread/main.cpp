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
}

auto doWork(int maxThreads)
{
    auto threads = std::vector<std::jthread>();
    for (int i = 0; i < maxThreads - 1; i++)
    {
        threads.push_back(std::jthread(heavyComputation));
    }

    prepareForResults();
}

int main()
{
    doWork(std::thread::hardware_concurrency());
    return 0;
}