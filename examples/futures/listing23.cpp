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

    auto sharedFuture = future.share(); // create shared_future from future
    
    std::print("First result: {}\n", sharedFuture.get()); // get the result
    std::this_thread::sleep_for(1s);
    std::print("Second result: {}\n", sharedFuture.get()); // get the result again
}

int main()
{
    doWork();
    return 0;
}
// Listing 23: Using shared_future