#include <chrono>
#include <future>
#include <mutex>
#include <print>
#include <vector>

class SharedPrinter {
 public:
  auto printDocument(std::chrono::milliseconds waitFor,
                     unsigned attempts) -> bool {
    auto guard = std::unique_lock(m_, std::defer_lock);
    for (auto i = 0u; i < attempts; ++i) {
      auto lockSuccessful = guard.try_lock_for(waitFor);
      if (lockSuccessful) {
        std::println("Printing!");
        std::this_thread::sleep_for(printing_time);
        return true;
      }
    }
    return false;
  }

 private:
  mutable std::timed_mutex m_;
  std::chrono::milliseconds printing_time{400};
};

int main() {
  auto futures = std::vector<std::future<void>>{};
  auto threadCount = std::thread::hardware_concurrency();
  futures.reserve(threadCount);

  auto sharedPrinter = SharedPrinter{};
  const auto timeout = std::chrono::milliseconds(200);

  for (int i = 0; i < threadCount; i++) {
    futures.emplace_back(
        std::async([&sharedPrinter, timeout, attempts = i] {
          auto result =
              sharedPrinter.printDocument(timeout, attempts);
          if (not result) {
            std::println("Failed to print!");
          }
        }));
  }
  std::for_each(futures.begin(), futures.end(),
                [](auto& f) { f.wait(); });
  return 0;
}
// Listing 4.4: Locking with timeout