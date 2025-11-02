#include <future>
#include <print>
#include <thread>

auto throwingOperation() {
  throw std::runtime_error("Computation error");
}

auto heavyComputation(std::promise<unsigned> promise) {
  std::println("Simulating computation in background");
  auto sum = 0u;
  for (auto i = 0u; i < 1'000'000'000u; ++i) {
    sum += i % 2;
  }
  promise.set_value(sum);
}

auto doWork() {
  std::println("Starting computation...");
  auto promise = std::promise<unsigned>();
  auto future = promise.get_future();
  auto jthread =
      std::jthread(heavyComputation, std::move(promise));
  std::println("Waiting for the result...");

  try {
    auto result = future.get();
    std::println("Computation finished. Result: {}", result);
  } catch (const std::runtime_error& e) {
    std::println("Caught exception: {}", e.what());
  }
}

int main() { doWork(); }
// Listing 2.2: Passing exceptions with future/promise