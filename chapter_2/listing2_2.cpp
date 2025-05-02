#include <chrono>
#include <future>
#include <print>
#include <thread>

using std::chrono_literals::operator""s;

auto throwingOperation() { 
  throw std::runtime_error("Computation error"); 
}

auto heavyComputation(std::promise<int> promise) {
  std::this_thread::sleep_for(3s);
  try {
    throwingOperation();
    promise.set_value(21);
  } catch (const std::exception& e) {
    promise.set_exception(std::current_exception());
  }
}

auto doWork() {
  std::println("Starting computation...");
  auto promise = std::promise<int>();
  auto future = promise.get_future();
  auto jthread = std::jthread(heavyComputation, std::move(promise));
  std::println("Waiting for the result...");

  try {
    auto result = future.get();
    std::println("Result: {}", result);
  } catch (const std::runtime_error& e) {
    std::println("Caught exception: {}", e.what());
  }
}

int main() {
  doWork();
  return 0;
}
// Listing 2.2: Passing exceptions with future/promise