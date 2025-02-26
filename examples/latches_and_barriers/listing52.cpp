#include <print>
#include <thread>
#include <vector>
#include <latch>
#include <functional>

using std::chrono_literals::operator""ms;

// barrier concept 
template<typename T>
concept RepeatedlyArrivable = requires(T& b, typename T::arrival_token token) {
    { b.arrive_and_wait() } -> std::same_as<void>; // must have arrive_and_wait()
    { b.arrive() } -> std::same_as<typename T::arrival_token>; // must have arrive()
    requires std::is_move_constructible_v<typename T::arrival_token>; // token must be movable
};

template <RepeatedlyArrivable Barrier>
auto worker(Barrier& barrier, int id) {
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
    auto barrier = std::latch(numWorkers);

    auto workers = std::vector<std::jthread>{};
    for (int i = 1; i <= numWorkers; ++i) {
        workers.emplace_back(worker<decltype(barrier)>, std::ref(barrier), i);
    }

    std::print("All workers finished\n");
    return 0;
}
// Listing 52: barrier worker threads with barrier concept