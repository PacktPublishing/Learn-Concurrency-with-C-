#include <print>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

std::queue<int> queue;
std::mutex m;
std::condition_variable cv;
bool data_ready = false;

auto producer() {
    {
        const auto lock = std::lock_guard(m);
        auto value = 21;
        std::print("Pushing value: {}\n", value);
        queue.push(value);
        data_ready = true;
    }
    cv.notify_one(); // notify consumer thread
}

auto consumer() {
    auto lock = std::unique_lock(m);
    cv.wait(lock, [] { return data_ready; }); // wait for notification
    int value = queue.front();
    queue.pop();
    lock.unlock();
    std::print("Consumed value: {}\n", value);
}


int main() {
    auto t1 = std::jthread(consumer);
    auto t2 = std::jthread(producer);
    return 0;
}
// Listing 47: Producer-Consumer threads with notification mechanism