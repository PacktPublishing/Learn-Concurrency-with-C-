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

  try {
    prepareForResults();
  } catch (const std::exception &e) {
    for (auto& t : threads) {
      auto id = t.get_id();
      std::println("Error detected, thread: {}", id);
      t.join();
    }
    throw;
  }

  for (auto& t : threads) {
    std::println("Joining thread: {}", t.get_id());
    t.join();
  }
}

int main() {
  try {
    doWork(std::thread::hardware_concurrency() - 1);
  } catch (const std::exception &e) {
    std::print("Caught exception: {}\n", e.what());
  }
  return 0;
}
// Listing 5: prepareForResults() wrapped in a try-catch block