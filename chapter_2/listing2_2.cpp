#include <future>
#include <print>
#include <thread>

auto heavyComputation(std::promise<int> promise) {
  try {
    std::println("Simulating computation in background");
    throw std::runtime_error("Computation error");
  } catch (const std::runtime_error& e) {
    promise.set_exception(std::current_exception());
  }
}

auto doWork() {
  std::println("Starting computation...");
  auto promise = std::promise<int>();
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