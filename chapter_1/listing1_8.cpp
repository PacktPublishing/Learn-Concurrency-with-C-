#include <atomic>
#include <iostream>
#include <print>
#include <thread>
#include <unordered_map>
#include <vector>

auto getMaxNrOfBackgroundThreads() {
  const auto maxThreads = std::thread::hardware_concurrency();
  return maxThreads > 1 ? maxThreads - 1 : 1;
}

using SpecialCharacters = std::unordered_map<char, std::string>;

class App {
 public:
  explicit App(unsigned maxThreads)
      : maxBackgroundThreads_(maxThreads),
        specialChars_{{'q', "quit"},
                      {'e', "throw an exception"}} {
    backgroundThreads_.reserve(maxThreads);
    std::println("Using {} threads for background computation",
                 maxBackgroundThreads_);
  }

  auto launch() -> void {
    startBackgroundThreads();
    startUserInteraction();
  }

  ~App() {
    cancelAllOperations();
    joinBackgroundThreads();
  }

 private:
  auto startBackgroundThreads() -> void {
    for (unsigned i = 0; i < maxBackgroundThreads_; ++i) {
      backgroundThreads_.emplace_back(&App::heavyComputation,
                                      this, i + 2);
    }
  }
  auto startUserInteraction() -> void {
    try {
      talkWithUser();
    } catch (const std::exception& e) {
      std::println("Exception occurred: {}", e.what());
    }
  }

  auto heavyComputation(int modulo) -> void {
    std::println("Simulating computation in background");
    auto sum = 0u;
    for (auto i = 0u; i < 1'000'000'000u; ++i) {
      sum += i % modulo;
      if (cancelFlag_.test()) {
        break;
      }
    }
    std::println("Computation finished. Result: {}", sum);
  }

  auto talkWithUser() -> void {
    std::println("Enter character to process (or 'q' to quit):");
    char input;
    while (not cancelFlag_.test()) {
      std::cin >> input;
      if (input == 'q') {
        cancelFlag_.test_and_set();
        break;
      }
      if (input == 'e') {
        throw std::runtime_error("Simulated error");
      }
      std::println("You entered: {}", input);
    }
  }

  auto cancelAllOperations() noexcept -> void {
    cancelFlag_.test_and_set();
  }

  auto joinBackgroundThreads() noexcept -> void {
    try {
      std::println("Joining background threads...");
      for (auto& thread : backgroundThreads_) {
        thread.join();
      }
      std::println("All background threads have finished.");
    } catch (const std::exception& e) {
      std::println("Error joining threads: {}", e.what());
    }
  }

  auto printSpecialCharacters() {
    std::println("Special characters:");
    for (const auto& [ch, action] : specialChars_) {
      std::println(" > {} to {}", ch, action);
    }
  }

  unsigned maxBackgroundThreads_;
  std::vector<std::thread> backgroundThreads_;
  std::atomic_flag cancelFlag_{};
  SpecialCharacters specialChars_;
};

int main() {
  const auto maxThreads = getMaxNrOfBackgroundThreads();
  auto app = App{maxThreads};
  app.launch();
}
// Listing 1.8: Thread cancellation