#include <chrono>
#include <future>
#include <print>
#include <thread>

using std::chrono_literals::operator""s;

auto heavyComputation(std::promise<int> promise) {
  promise.set_value_at_thread_exit(21);
  std::this_thread::sleep_for(3s);
}

int main() {
  auto promise = std::promise<int>();
  auto future = promise.get_future();
  std::println("Starting work on a detached thread");

  auto t = std::thread(heavyComputation, std::move(promise));
  t.detach();

  std::println("Waiting...");
  future.wait();
  std::println("Thread complete, result: {}", future.get());
  return 0;
}
// Listing 2.7: Starting a detached thread with a promise