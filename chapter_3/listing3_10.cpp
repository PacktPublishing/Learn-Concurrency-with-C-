#include <mutex>
#include <print>
#include <thread>

int main() {
  using std::chrono_literals::operator""ms;

  std::mutex m1;
  std::mutex m2;

  auto t1 = std::jthread([&m1, &m2]() {
    std::println("Acquiring resource #1!");
    auto guard1 = std::lock_guard(m1);
    std::this_thread::sleep_for(10ms);
    std::println("Acquiring resource #2!");
    auto guard2 = std::lock_guard(m2);
  });

  auto t2 = std::jthread([&m1, &m2]() {
    std::println("Acquiring resource #2!");
    auto guard2 = std::lock_guard(m2);
    std::this_thread::sleep_for(10ms);
    std::println("Acquiring resource #1!");
    auto guard1 = std::lock_guard(m1);
  });

  return 0;
}
// Listing 3.10: Threads contending for same resources
