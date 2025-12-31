#include <future>
#include <print>
#include <shared_mutex>
#include <vector>

std::mutex m;
int globalResource = 0;

using std::chrono_literals::operator""s;

auto modifyResource() {
  auto lock = std::unique_lock(m, std::defer_lock);
  while (not lock.try_lock()) {
    std::print("Failed to lock mutex\n");
    std::this_thread::sleep_for(1s);
  }
  std::print("Mutex locked\n");
  ++globalResource;
}

int main() {
  auto futures = std::vector<std::future<void>>{};
  auto threadsNr = std::thread::hardware_concurrency();

  std::print("Launching {} threads, global resource value: {}\n",
             threadsNr, globalResource);
  for (int i = 0; i < threadsNr; i++) {
    std::print("Launching thread\n");
    futures.emplace_back(std::async(modifyResource));
  }

  std::for_each(futures.begin(), futures.end(),
                [](auto& f) { f.wait(); });
  std::print("Threads finished, global resource value: {}\n",
             globalResource);
  return 0;
}
// Listing 4.6: Defer lock