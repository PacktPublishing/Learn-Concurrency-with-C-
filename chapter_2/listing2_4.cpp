#include <chrono>
#include <future>
#include <print>
#include <thread>
#include <vector>

using std::chrono_literals::operator""s;

auto heavyComputation(int i) {
  std::this_thread::sleep_for(3s);
  return i + 10;
}

auto doWork(int maxThreads) {
  std::println("Starting computation...");
  auto resultFutures = std::vector<std::future<int>>{};
  for (int i = 0; i < maxThreads; i++) {
    resultFutures.emplace_back(
        std::async(std::launch::async, heavyComputation, i));
  }

  std::println("Waiting for the results...");
  for (auto& f : resultFutures) {
    try {
      std::println("Result: {}", f.get());
    } catch (const std::exception& e) {
      std::println("Caught exception: {}", e.what());
    }
  }
}

int main() {
  doWork(std::thread::hardware_concurrency());
  return 0;
}
// Listing 2.4: Passing future/promise values with async