#include <print>
#include <thread>
#include <future>
#include <chrono>

using std::chrono_literals::operator""s;

auto throwingOperation()
{
    throw std::runtime_error("Computation error");
}

auto heavyComputation(std::promise<int> promise)
{
    std::this_thread::sleep_for(3s); // simulate computation
    try
    {
        throwingOperation(); // throw exception before setting value
        promise.set_value(21);
    }
    catch (const std::exception& e)
    {
        promise.set_exception(std::current_exception()); // store exception in promise
    }
}

auto doWork()
{
    std::print("Starting computation...\n");
    auto promise = std::promise<int>(); // create promise
    auto future = promise.get_future(); // create future associated with the promise
    auto jthread = std::jthread(heavyComputation, std::move(promise)); // move the promise, start computation
    std::print("Waiting for the result...\n");
    
    try
    {
        auto result = future.get(); // await result, get re-thrown exception instead
        std::print("Result: {}\n", result);
    }
    catch (const std::runtime_error& e)
    {
        std::print("Caught exception: {}\n", e.what());
    }
}

int main()
{
    doWork();
    return 0;
}
// Listing 15: Passing exceptions with future/promise