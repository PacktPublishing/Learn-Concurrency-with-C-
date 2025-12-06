#include <print>
#include <thread>

auto thread_func1(std::thread& t2) {
  std::println("Thread 1 started");
  t2.join();
  std::println("Thread 1 finished");
}

auto thread_func2(std::thread& t1) {
  std::println("Thread 2 started");
  t1.join();
  std::println("Thread 2 finished");
}

int main() {
  auto t1 = std::thread{};
  auto t2 = std::thread{};
  t1 = std::thread(thread_func1, std::ref(t2));
  t2 = std::thread(thread_func2, std::ref(t1));

  t1.join();
  t2.join();

  std::println("Main thread finished");
  return 0;
}
// Listing 3.9: Deadlock: threads stuck waiting for each other