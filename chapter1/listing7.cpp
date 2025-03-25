#include <print>
#include <thread>
#include <vector>
#include <atomic>

using std::chrono_literals::operator""s;

std::atomic_flag cancel_flag{};

auto heavyComputation() {
  auto id = std::this_thread::get_id();
  while (not cancel_flag.test()) {
    std::println("Thread {} is processing...", id);
    std::this_thread::sleep_for(1s);
  }
}

auto doWork(int maxThreads) {
  auto threads = std::vector<std::thread>{};
  threads.reserve(maxThreads);

  for (int i = 0; i < maxThreads; i++) {
    threads.emplace_back(heavyComputation);
  }

  try {
    std::this_thread::sleep_for(5s);
    cancel_flag.test_and_set();
  } catch (const std::exception &e) {
    for (auto &t : threads) {
      t.join();
    }
    throw;
  }

  for (auto& t : threads) {
    t.join();
  }
}

int main() {
  doWork(std::thread::hardware_concurrency() - 1);
  return 0;
}
// Listing 7: Thread cancellation