#include <future>
#include <mutex>
#include <print>
#include <vector>

std::mutex m;
int globalResource = 0;

auto modifyResource() {
  const auto guard = std::lock_guard(m);
  ++globalResource;
}

int main() {
  auto futures = std::vector<std::future<void>>{};
  auto threadsNr = std::thread::hardware_concurrency();

  std::print("Launching {} threads, global resource value: {}\n",
             threadsNr, globalResource);
  for (int i = 0; i < threadsNr; i++) {
    futures.emplace_back(std::async(modifyResource));
  }

  std::for_each(futures.begin(), futures.end(),
                [](auto& f) { f.wait(); });
  std::print("Threads finished, global resource value: {}\n",
             globalResource);
  return 0;
}
// Listing 4.1: Locking fundamentals using std::mutex and
// std::lock_guard