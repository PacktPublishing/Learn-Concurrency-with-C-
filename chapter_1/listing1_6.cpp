#include <iostream>
#include <print>
#include <thread>
#include <unordered_map>

auto heavyComputation() {
  std::println("Simulating computation in background");
  auto sum = 0u;
  for (auto i = 0u; i < 1'000'000'000u; ++i) {
    sum += i % 2;
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
  auto backgroundThread = std::thread(heavyComputation);
  talkWithUser();
  backgroundThread.join();
}

int main() {
  try {
    launchApp();
  } catch (const std::exception& e) {
    std::println("Exception occurred: {}", e.what());
  }
}
// Listing 1.6: Unsafe exception handling