#include <mutex>
#include <future>
#include <vector>
#include <print>

unsigned max_attempts = 3;
std::timed_mutex m;
int globalResource = 0;

using std::chrono_literals::operator""ms;

auto modifyResource() -> int {
    auto lock = std::unique_lock(m, std::try_to_lock); // initial, non-blocking attempt to lock
    std::print("Mutex locked: {}\n", lock.owns_lock());

    if (not lock.owns_lock()) {
        auto counter = max_attempts;
        while (not lock.try_lock_for(500ms)) { // repeated attempts to lock with timeout
            std::print("Failed to lock mutex\n");
            if(--counter == 0) {
                std::print("Max attempts reached\n");
                return 1; // resource modification failed
            }
        }
    }
    std::print("Mutex locked: {}\n", lock.owns_lock());
    ++globalResource;
    return 0; // resource modification successful
}

int main() {
    auto futures = std::vector<std::future<int>>{};
    auto threadsNr = std::thread::hardware_concurrency();

    std::print("Launching {} threads, global resource value: {}\n", threadsNr, globalResource);
    for (int i = 0; i < threadsNr; i++)
    {
        futures.emplace_back(std::async(modifyResource));
    }

    const auto failedThreads = std::count_if(futures.begin(), futures.end(), [](auto& f) { return f.get() == 1; }); // wait for completion and gather failed info
    std::print("Threads finished, global resource value: {}\n", globalResource);
    std::print("Thread that failed to modify resource: {}\n", failedThreads);
    return 0;
}
// Listing 36: Locking with timeout