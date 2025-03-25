#include <print>
#include <thread>
#include <vector>
#include <atomic>
#include <stop_token>

using std::chrono_literals::operator""s;

auto heavyComputation(std::stop_token stopToken) {
  const auto stopCallback = std::stop_callback{
    stopToken, 
    []{
      auto id = std::this_thread::get_id();
      std::println("Thread: {} is stopping...", id);
    }
  };

  auto id = std::this_thread::get_id();
  while (not stopToken.stop_requested()) {
    std::println("Thread {} is processing...", id);
    std::this_thread::sleep_for(1s);
  }
}

auto doWork(int maxThreads) {
  const auto stopSource = std::stop_source{};
  auto jthreads = std::vector<std::jthread>{};
  jthreads.reserve(maxThreads);
  for (int i = 0; i < maxThreads; i++) {
    jthreads.emplace_back(heavyComputation, stopSource.get_token());
  }

  std::this_thread::sleep_for(5s);
  stopSource.request_stop();
}

int main() {
  std::println("Main thread id: {}", std::this_thread::get_id());
  doWork(std::thread::hardware_concurrency());
  return 0;
}
// Listing 12: stop callback moved inside jthread