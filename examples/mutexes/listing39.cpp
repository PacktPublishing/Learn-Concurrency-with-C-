#include <shared_mutex> // separate header
#include <future>
#include <vector>
#include <print>

std::shared_mutex m;
int globalResource = 0;

using std::chrono_literals::operator""s;

auto modifyResource() -> void {
    auto lock = std::unique_lock(m); // unique lock for writers
    std::print("Modifying resource\n");
    globalResource += 21;
    std::this_thread::sleep_for(1s); // simulate longer operations
    std::print("Resource modified: {}\n", globalResource);
}

auto readReasource() -> void {
    auto lock = std::shared_lock(m); // shared lock for readers
    std::print("Reading resource: {}\n", globalResource);
}

int main() {
    auto futures = std::vector<std::future<void>>{};
    auto threadsNr = std::thread::hardware_concurrency();

    std::print("Launching {} threads, global resource value: {}\n", threadsNr, globalResource);
    for (int i = 0; i < threadsNr; i++)
    {
        if (i % (threadsNr / 2)) // low number of threads will write, the rest will read
        {
            std::print("Launching reader thread\n");
            futures.emplace_back(std::async(readReasource));
        } 
        else
        {
            std::print("Launching writer thread\n");
            futures.emplace_back(std::async(modifyResource));
        }
    }

    std::for_each(futures.begin(), futures.end(), [](auto& f) { f.wait(); }); // wait for completion
    std::print("Threads finished, global resource value: {}\n", globalResource);
    return 0;
}
// Listing 39: Shared mutex