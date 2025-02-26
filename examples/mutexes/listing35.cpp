#include <mutex>
#include <future>
#include <vector>
#include <print>

std::mutex m;
int globalResource = 0;

auto modifyResource() {
    const auto guard = std::lock_guard(m); // m.lock() called on guard construction
    ++globalResource; // modify resource
    // m.unlock() called on guard destruction
}

int main() {
    auto futures = std::vector<std::future<void>>{};
    auto threadsNr = std::thread::hardware_concurrency();

    std::print("Launching {} threads, global resource value: {}\n", threadsNr, globalResource);
    for (int i = 0; i < threadsNr; i++)
    {
        futures.emplace_back(std::async(modifyResource));
    }

    std::for_each(futures.begin(), futures.end(), [](auto& f) { f.wait(); }); // wait for completion
    std::print("Threads finished, global resource value: {}\n", globalResource);
    return 0;
}
// Listing 35: Locking fundamentals using std::mutex and std::lock_guard