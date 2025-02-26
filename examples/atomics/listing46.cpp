#include <atomic>
#include <future>
#include <vector>
#include <print>

int globalResource{0};

auto modifyResource() {
    auto safeAccess = std::atomic_ref{globalResource};
    ++safeAccess;
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
    std::print("Threads finished, global resource value: {}\n", std::atomic_ref{globalResource}.load());
    return 0;
}
// Listing 46: Modifying resource with atomic operations