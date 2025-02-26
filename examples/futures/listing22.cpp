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
    auto promise = std::promise<int>();
    auto future = promise.get_future();
    auto jthread = std::jthread(heavyComputation, std::move(promise));
    std::print("Waiting for the result...\n");

    auto result = future.get(); // get the result

    try
    {
        future.get(); // get the result again - ERROR
    }
    catch (const std::future_error& e)
    {
        std::print("Error: {}\n", e.what());
    }
}

int main()
{
    doWork();
    return 0;
}
// Listing 22: Obtaining the result from a future twice