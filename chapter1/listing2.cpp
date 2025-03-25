#include <print>
#include <thread>

int main() {
  auto lambda = [](int val) {
    std::println("Hello from thread! Value: {}", val);
  };
  auto myThread = std::thread(lambda, 21);
  myThread.join();
  return 0;
}
// Listing 2: Thread creation with parameters passing