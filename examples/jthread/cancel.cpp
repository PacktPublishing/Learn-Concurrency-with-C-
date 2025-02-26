#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <stop_token>

using std::chrono_literals::operator""s;

auto heavyComputation(std::stop_token stopToken)
{
    std::cout << "this function is processing data\n";
    while (not stopToken.stop_requested())
    {
        // compute until cancel detected
        std::cout << "computing...\n";
        std::this_thread::sleep_for(1s);
    }
}

auto doWork(int maxThreads)
{
    auto jthreads = std::vector<std::jthread>();
    for (int i = 0; i < maxThreads; i++)
    {
        jthreads.emplace_back(heavyComputation);
    }

    std::this_thread::sleep_for(5s); // simulate some work

    for (auto& jt : jthreads)
    {
        std::stop_callback callback (jt.get_stop_token(), []() {
            std::cout << "stop callback called\n";
        });
        jt.request_stop(); // stop every jthread
    }
}

int main()
{
    doWork(2);
    return 0;
}