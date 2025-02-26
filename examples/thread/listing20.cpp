#include <print>
#include <thread>
#include <future>
#include <chrono>

using std::chrono_literals::operator""s;

auto heavyComputation(std::promise<int> promise)
{
    promise.set_value_at_thread_exit(21); // store value at thread finish
    std::this_thread::sleep_for(3s); // simulate computation
}

int main() {
    auto promise = std::promise<int>();
    auto future = promise.get_future();
    std::print("Starting work on a detached thread\n");

    auto t = std::thread(heavyComputation, std::move(promise));
    t.detach(); // detach the thread instead of joining

    std::print("Waiting...\n");
    future.wait();
    std::print("Thread complete, result: {}\n", future.get());
    return 0;
}
// Listing 20: Starting a detached thread with a promise