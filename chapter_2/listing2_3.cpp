#include <chrono>
#include <future>
#include <print>
#include <thread>

using std::chrono_literals::operator""s;

auto brokenComputation(std::promise<int> promise) {
  std::this_thread::sleep_for(3s);
}

auto doWork() {
  std::println("Starting computation...");
  auto brokenPromise = std::promise<int>();
  auto disapppointedFuture = brokenPromise.get_future();
  auto jthread = std::jthread(brokenComputation, std::move(brokenPromise));
  std::println("Waiting for the result...");

  while (disapppointedFuture.wait_for(1s) != std::future_status::ready) {
    std::println("Still waiting...");
  }

  try {
    auto result = disapppointedFuture.get();
    std::println("Result: {}", result);
  } catch (const std::future_error &e) {
    std::println("Caught exception: {}", e.what());
  }
}

int main() {
  doWork();
  return 0;
}
// Listing 2.3: Expressing an unfulfilled result