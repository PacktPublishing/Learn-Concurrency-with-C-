#include <future>
#include <mutex>
#include <print>
#include <vector>

class SharedResource {
 public:
  auto get() const {
    const auto guard = std::lock_guard(m_);
    return value_;
  }
  auto set(int val) {
    const auto guard = std::lock_guard(m_);
    value_ = val;
  }
  auto increment() {
    const auto guard = std::lock_guard(m_);
    ++value_;
  }

 private:
  mutable std::mutex m_;
  int value_;
};

int main() {
  auto futures = std::vector<std::future<void>>{};
  auto threadCount = std::thread::hardware_concurrency();
  futures.reserve(threadCount);

  auto sharedResource = SharedResource{21};

  std::println("Launching {} threads, global resource value: {}",
               threadCount, sharedResource.get());
  for (int i = 0; i < threadCount; i++) {
    futures.emplace_back(std::async(
        [&sharedResource] { sharedResource.increment(); }));
  }

  std::for_each(futures.begin(), futures.end(),
                [](auto& f) { f.wait(); });
  std::println("Threads finished, global resource value: {}",
               sharedResource.get());
  return 0;
}
// Listing 4.2: Making operations indivisible