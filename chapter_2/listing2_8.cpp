#include <chrono>
#include <future>
#include <print>
#include <thread>

using std::chrono_literals::operator""s;

auto continuousProcessing() {
  std::println("Processing...");
  std::this_thread::sleep_for(5s);
  throw std::runtime_error("Processing error");
}

auto cleanup() {
  std::println("Cleaning up...");
  std::this_thread::sleep_for(2s);
}

auto heavyComputation(std::stop_token stopToken,
                      std::promise<int> p) {
  while (not stopToken.stop_requested()) {
    try {
      continuousProcessing();
      p.set_value_at_thread_exit(21);
    } catch (const std::exception& e) {
      std::println("Caught exception: {}", e.what());
      p.set_exception_at_thread_exit(std::current_exception());
    }
    cleanup();
    return;
  }
}

int main() {
  auto promise = std::promise<int>();
  auto future = promise.get_future();
  std::println("Starting work on a detached thread");

  auto t = std::jthread(heavyComputation, std::move(promise));
  t.detach();

  std::println("Waiting without joining thread...");
  std::this_thread::sleep_for(3s);
  std::println("Requesting execution stop...");
  t.request_stop();

  try {
    std::println("Obtaining result...");
    future.get();
  } catch (const std::exception& e) {
    std::println("Something happened: {}", e.what());
  }
  return 0;
}
// Listing 2.8: Detached jthread and exception at thread exit