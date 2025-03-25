#include <print>
#include <thread>
#include <vector>

using std::chrono_literals::operator""s;

auto prepareForResults() {
  auto id = std::this_thread::get_id();
  std::println("Thread {} is preparing...", id);
}

auto heavyComputation(int i) {
  auto id = std::this_thread::get_id();
  std::println("Thread {} is processing...", id);
  std::this_thread::sleep_for(i * 1s);
}

auto doWork(int maxThreads) {
  auto threads = std::vector<std::thread>{};
  threads.reserve(maxThreads);

  for (int i = 0; i < maxThreads; i++) {
    threads.emplace_back(heavyComputation, i);
  }

  prepareForResults();

  for (auto& t : threads) {
    std::println("Joining thread: {}", t.get_id());
    t.join();
  }
}

int main() {
  doWork(std::thread::hardware_concurrency() - 1);
  return 0;
}
// Listing 4: Starting multiple threads