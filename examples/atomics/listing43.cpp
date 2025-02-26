#include <atomic>
#include <future>
#include <vector>
#include <print>

std::atomic<int> globalResource{0};

auto modifyResource() {
    ++globalResource; // no need for mutexes, thread-safe atomic operation
}

int main() {
    auto futures = std::vector<std::future<void>>{};
    auto threadsNr = std::thread::hardware_concurrency();

    std::print("Launching {} threads\n", threadsNr);
    for (int i = 0; i < threadsNr; i++)
    {
        futures.emplace_back(std::async(modifyResource));
    }

    std::for_each(futures.begin(), futures.end(), [](auto& f) { f.wait(); }); // wait for completion
    std::print("Threads finished, global resource value: {}\n", globalResource.load());
    return 0;
}
// Listing 43: Modifying resource with atomic operations