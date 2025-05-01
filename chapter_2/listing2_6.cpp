#include <chrono>
#include <future>
#include <print>
#include <thread>

using std::chrono_literals::operator""s;

auto heavyComputation() {
  std::this_thread::sleep_for(3s);
  return 21;
}

auto doWork() {
  std::println("Starting computation...");
  auto task = std::packaged_task<int()>(heavyComputation);
  auto future = task.get_future();
  auto jthread = std::jthread(std::move(task));
  std::println("Waiting for the result...");
  auto result = future.get();
  std::println("Result: {}", result);
}

int main() {
  doWork();
  return 0;
}
// Listing 2.6: Packaged task