#include <print>
#include <thread>

auto heavyComputation() {
  std::println("Hello from thread!");
}

int main() {
  auto myThread = std::thread(heavyComputation);
  myThread.join();
  return 0;
}
// Listing 1: Simple thread creation