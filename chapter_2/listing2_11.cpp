#include <chrono>
#include <future>
#include <print>
#include <thread>

using std::chrono_literals::operator""s;

auto heavyComputation() -> int {
  std::this_thread::sleep_for(3s);
  return 21;
}

class LongtermProcess {
 public:
  auto launchProcess() -> std::shared_future<int> {
    const auto lock = std::lock_guard{m_};
    if (result_.has_value() and
        (*result_).wait_for(0s) != std::future_status::ready) {
      std::println("Process already running");
      return *result_;
    }
    std::println("Starting long-term process...");
    result_.reset();
    auto future =
        std::async(std::launch::async, heavyComputation);
    result_ = future.share();
    return *result_;
  }

 private:
  std::mutex m_;
  std::optional<std::shared_future<int>> result_;
};

int main() {
  LongtermProcess lp;

  auto t1 = std::jthread([&lp] {
    std::println("Thread 1 launching process...");
    auto result = lp.launchProcess();
    std::println("Thread 1 waiting for the result...");
    std::println("Thread 1 result: {}", result.get());
  });

  auto t2 = std::jthread([&lp] {
    std::println("Thread 2 launching process...");
    auto result = lp.launchProcess();
    std::println("Thread 2 waiting for the result...");
    std::println("Thread 2 result: {}", result.get());
  });

  auto t3 = std::jthread([&lp] {
    std::println("Thread 3 waiting before launch...");
    std::this_thread::sleep_for(5s);
    std::println("Thread 3 launching process...");
    auto result = lp.launchProcess();
    std::println("Thread 3 waiting for the result...");
    std::println("Thread 3 result: {}", result.get());
  });

  return 0;
}
// Listing 2.11: Using shared_future for single-launch, long-term
// processing