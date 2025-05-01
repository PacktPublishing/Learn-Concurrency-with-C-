#include <chrono>
#include <future>
#include <print>
#include <thread>

using std::chrono_literals::operator""s;

auto heavyComputation(std::stop_token stopToken, std::promise<int> promise) {
  try {
    throw std::runtime_error("Computation error");
  } catch (const std::exception& e) {
    promise.set_exception_at_thread_exit(
        std::current_exception());  // store exception at thread finish
  }

  while (not stopToken.stop_requested()) {
    std::this_thread::sleep_for(1s);  // simulate computation
  }
}

int main() {
  auto promise = std::promise<int>();
  auto future = promise.get_future();
  std::println("Starting work on a detached thread");

  auto t = std::jthread(heavyComputation, std::move(promise));

  std::println("Is jthread joinable before detach: {}", t.joinable());
  t.detach();
  std::println("Is jthread joinable after detach: {}", t.joinable());

  std::this_thread::sleep_for(1s);

  std::println("Is stop possible before request: {}",
               t.get_stop_source().stop_possible());
  std::println("Is stop requested: {}\n", t.get_stop_source().stop_requested());
  t.request_stop();
  std::println("Is stop possible after request: {}",
               t.get_stop_source().stop_possible());
  std::println("Is stop requested: {}\n", t.get_stop_source().stop_requested());

  std::this_thread::sleep_for(1s);

  try {
    std::println("Obtaining result...");
    future.get();
  } catch (const std::exception& e) {
    std::println("Something happened: {}", e.what());
  }
  return 0;
}
// Listing 2.8: Starting a detached jthread with a promise