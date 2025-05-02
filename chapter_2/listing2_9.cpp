#include <chrono>
#include <future>
#include <print>
#include <thread>

using std::chrono_literals::operator""s;

auto heavyComputation(std::promise<int> promise) {
  std::this_thread::sleep_for(3s);
  promise.set_value(21);
}

auto doWork() {
  std::println("Starting computation...");
  auto promise = std::promise<int>();
  auto future = promise.get_future();
  auto jthread = std::jthread(heavyComputation, std::move(promise));
  std::println("Waiting for the result...");

  for (auto i = 0; i < 10; ++i) {
    try {
      auto result = future.get();
      std::println("Do something with: {}", result);
      result += 10;
    } catch (const std::future_error& e) {
      std::println("Error: {}", e.what());
    }
  }
}

int main() {
  doWork();
  return 0;
}
// Listing 2.9: Obtaining the result from a future twice