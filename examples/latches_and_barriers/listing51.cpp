#include <print>
#include <thread>
#include <vector>
#include <barrier>
#include <functional>

using std::chrono_literals::operator""ms;

template <typename OnCompletion>
auto worker(std::barrier<OnCompletion>& barrier, int id) {
    std::print("Worker {} active\n", id);

    std::this_thread::sleep_for(200ms); // simulate phase 1
    std::print("Worker {} arriving at first checkpoint\n", id);
    barrier.arrive_and_wait();

    std::this_thread::sleep_for(300ms); // simulate phase 2
    std::print("Worker {} arriving at second checkpoint\n", id);
    barrier.arrive_and_wait();

    std::print("Worker {} finished\n", id);
}

int main() {
    const int numWorkers = 3;
    auto onCompletion = [] { std::print("... Phase complete ...\n"); };
    auto barrier = std::barrier(numWorkers, onCompletion);

    auto workers = std::vector<std::jthread>{};
    for (int i = 1; i <= numWorkers; ++i) {
        workers.emplace_back(worker<decltype(onCompletion)>, std::ref(barrier), i);
    }

    std::print("All workers finished\n");
    return 0;
}
// Listing 51: barrier threads with template completion callback