#include <print>
#include <thread>
#include <future>
#include <chrono>

using std::chrono_literals::operator""s;

auto heavyComputation()
{
    std::this_thread::sleep_for(3s); // simulate computation
    return 21;
}

auto doWork()
{
    std::print("Starting computation...\n");
    auto task = std::packaged_task<int()>(heavyComputation);
    auto future = task.get_future();
    auto jthread = std::jthread(std::move(task)); // move the task, start computation
    std::print("Waiting for the result...\n");
    auto result = future.get();
    std::print("Result: {}\n", result);
}

int main()
{
    doWork();
    return 0;
}
// Listing 19: Packaged task