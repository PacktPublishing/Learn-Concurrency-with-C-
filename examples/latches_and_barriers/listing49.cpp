#include <print>
#include <thread>
#include <vector>
#include <latch>

using std::chrono_literals::operator""ms;

auto worker(std::latch& latch, int id) {
    std::print("Worker {} active\n", id);
    std::this_thread::sleep_for(200ms); // simulate work
    std::print("Worker {} arriving at the latch\n", id);
    latch.arrive_and_wait(); // decrement latch counter
    std::print("Worker {} detected phase completion\n", id);
}

int main() {
    const int numWorkers = 3;
    auto latch = std::latch(numWorkers); // latch should wait for 3 workers

    auto workers = std::vector<std::jthread>{};
    for (int i = 1; i <= numWorkers; ++i) {
        workers.emplace_back(worker, std::ref(latch), i);
    }

    latch.wait(); // wait until all workers finish

    std::print("All workers finished\n");
    return 0;
}
// Listing 49: worker threads synchronized using a latch