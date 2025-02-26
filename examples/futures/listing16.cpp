#include <print>
#include <thread>
#include <future>
#include <chrono>

using std::chrono_literals::operator""s;

auto brokenComputation(std::promise<int> promise)
{
    std::this_thread::sleep_for(3s);
    // promise is destroyed and never fulfilled
}

auto doWork() 
{
    std::print("Starting computation...\n");
    auto brokenPromise = std::promise<int>();
    auto disapppointedFuture = brokenPromise.get_future();
    auto jthread = std::jthread(brokenComputation, std::move(brokenPromise));
    std::print("Waiting for the result...\n");

    while (disapppointedFuture.wait_for(1s) != std::future_status::ready)
    {
        std::print("Still waiting...\n");
    }

    try
    {
        auto result = disapppointedFuture.get();
        std::print("Result: {}\n", result);
    }
    catch (const std::future_error& e)
    {
        std::print("Caught exception: {}\n", e.what());
    }
}

int main()
{
    doWork();
    return 0;
}
// Listing 16: Expressing an unfulfilled result