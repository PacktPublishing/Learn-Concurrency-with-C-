#include <print>
#include <thread>
#include <future>
#include <chrono>

using std::chrono_literals::operator""s;

auto heavyComputation(std::promise<int> promise)
{
    std::this_thread::sleep_for(3s); // simulate computation
    promise.set_value(21); // store value
}

auto doWork() 
{
    std::print("Starting computation...\n");
    auto promise = std::promise<int>(); // create promise
    auto future = promise.get_future(); // create future associated with the promise
    auto jthread = std::jthread(heavyComputation, std::move(promise)); // move the promise, start computation
    std::print("Waiting for the result...\n");
    auto result = future.get(); // await and retrieve result
    std::print("Result: {}\n", result);
}

int main()
{
    doWork();
    return 0;
}
// Listing 14: Passing results with future/promise