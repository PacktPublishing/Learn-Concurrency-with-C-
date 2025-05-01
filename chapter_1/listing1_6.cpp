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
  auto jthreads = std::vector<std::jthread>{};
  jthreads.reserve(maxThreads);

  for (int i = 0; i < maxThreads; i++)
  {
    jthreads.emplace_back(heavyComputation, i);
  }
  prepareForResults();
}

int main() {
  try {
    doWork(std::thread::hardware_concurrency() - 1);
  } catch (const std::exception &e) {
    std::print("Caught exception: {}\n", e.what());
  }
  return 0;
}
// Listing 1.6: DoWork function with an std::jthread