#include <atomic>
#include <functional>
#include <iostream>
#include <print>
#include <stop_token>
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

  ~App() { cancelAllOperations(); }

 private:
  auto startBackgroundThreads() -> void {
    for (unsigned i = 0; i < maxBackgroundThreads_; ++i) {
      backgroundThreads_.emplace_back(
          std::bind_front(&App::heavyComputation, this,
                          stopSource_.get_token()),
          i + 2);
    }
  }

  auto heavyComputation(std::stop_token token, int modulo)
      -> void {
    std::println("Simulating computation in background");
    auto sum = 0u;
    for (auto i = 0u; i < 1'000'000'000u; ++i) {
      sum += i % modulo;
      if (token.stop_requested()) {
        break;
      }
    }
    std::println("Computation finished. Result: {}", sum);
  }

  auto startUserInteraction() -> void {
    const auto stopCallback = std::stop_callback{
        stopSource_.get_token(),
        [] { std::println("App stopped successfully"); }};
    try {
      talkWithUser();
    } catch (const std::exception& e) {
      std::println("Exception occurred: {}", e.what());
    }
  }

  auto talkWithUser() -> void {
    std::println("Enter character to process (or 'q' to quit):");
    char input;
    while (not stopSource_.stop_requested()) {
      std::cin >> input;
      if (input == 'q') {
        cancelAllOperations();
        break;
      }
      if (input == 'e') {
        throw std::runtime_error("Simulated error");
      }
      std::println("You entered: {}", input);
    }
  }

  auto cancelAllOperations() noexcept -> void {
    stopSource_.request_stop();
  }

  auto printSpecialCharacters() {
    std::println("Special characters:");
    for (const auto& [ch, action] : specialChars_) {
      std::println(" > {} to {}", ch, action);
    }
  }

  unsigned maxBackgroundThreads_;
  std::vector<std::jthread> backgroundThreads_;
  std::stop_source stopSource_;
  SpecialCharacters specialChars_;
};

int main() {
  const auto maxThreads = getMaxNrOfBackgroundThreads();
  auto app = App{maxThreads};
  app.launch();
}
// Listing 1.11: Stop callback example