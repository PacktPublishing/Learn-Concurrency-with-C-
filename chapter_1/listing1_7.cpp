#include <iostream>
#include <print>
#include <thread>
#include <vector>
#include <unordered_map>

auto getMaxNrOfBackgroundThreads() {
  const auto maxThreads = std::thread::hardware_concurrency();
  return maxThreads > 1 ? maxThreads - 1 : 1;
}

auto heavyComputation(int modulo) {
  std::println("Simulating computation in background");
  auto sum = 0u;
  for (auto i = 0u; i < 1'000'000'000u; ++i) {
    sum += i % modulo;
  }
  std::println("Computation finished. Result: {}", sum);
}

using SpecialCharacters = std::unordered_map<char, std::string>;
const SpecialCharacters SPECIAL_CHARS = {
    {'q', "quit"}, {'e', "throw an exception"}};

auto printSpecialCharacters() {
  std::println("Special characters:");
  for (const auto& [ch, action] : SPECIAL_CHARS) {
    std::println(" > {} to {}", ch, action);
  }
}

auto talkWithUser() {
  std::println("Enter character, press ENTER to process");
  printSpecialCharacters();
  char input;
  while (std::cin >> input) {
    if (input == 'q') {
      break;
    }
    if (input == 'e') {
      throw std::runtime_error("Simulated error");
    }
    std::println("You entered: {}", input);
  }
}

auto launchApp() {
  const auto maxBackgroundThreads = getMaxNrOfBackgroundThreads();
  std::println("Using {} threads for background computation",
               maxBackgroundThreads);

  auto backgroundThreads = std::vector<std::jthread>{};
  backgroundThreads.reserve(maxBackgroundThreads);
  for (int i = 0; i < maxBackgroundThreads; i++) {
    backgroundThreads.emplace_back(heavyComputation, i + 2);
  }

  try {
    talkWithUser();
  } catch (const std::exception& e) {
    std::println("Exception occurred: {}", e.what());
  }
}

int main() { launchApp(); }
// Listing 1.7: Background computation with jthreads