#include <future>
#include <print>
#include <thread>

auto heavyComputation(std::promise<int> promise) {
  std::println("Simulating computation in background");
  auto sum = 0u;
  for (auto i = 0u; i < 1'000'000'000u; ++i) {
    sum += i % 2;
  }
  promise.set_value(sum);
}

auto doWork() {
  std::println("Starting computation...");
  auto promise = std::promise<int>();
  auto future = promise.get_future();
  auto jthread =
      std::jthread(heavyComputation, std::move(promise));
  std::println("Waiting for the result...");
  auto result = future.get();
  std::println("Computation finished. Result: {}", result);
}

int main() { doWork(); }
// Listing 2.1: Passing results with future/promise
