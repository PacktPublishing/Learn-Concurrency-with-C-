#include <print>
#include <thread>
#include <vector>

class Resource {
 public:
  auto getX() const { return x; }
  auto setX(int val) { x = val; }

 private:
  int x{21};
};

Resource resource{};

auto heavyComputation() {
  auto x_before = resource.getX();
  std::println("Value of x before: {}", x_before);
  auto new_x = x_before + 1;
  resource.setX(new_x);
  auto x_after = resource.getX();
  std::println("Value of x after: {}", x_after);
}

auto doWork(int maxThreads) {
  auto jthreads = std::vector<std::jthread>();
  for (int i = 0; i < maxThreads; i++) {
    jthreads.emplace_back(heavyComputation);
  }
}

int main() {
  auto nrOfThreads = std::thread::hardware_concurrency();
  std::println("Number of threads: {}", nrOfThreads);
  doWork(nrOfThreads);
  std::println("Final value of x: {}", resource.getX());
  return 0;
}
// Listing 3.8: Data race: incrementing x in multiple threads