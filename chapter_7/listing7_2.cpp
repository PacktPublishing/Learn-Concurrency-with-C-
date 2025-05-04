#include <mutex>
#include <print>
#include <thread>

template <typename Resource>
class Monitor {
 private:
  class AccessGuard {
   public:
    AccessGuard(Monitor& monitor)
        : monitor_{monitor}, resourceLock_{monitor_.mutex_} {
      std::println("AccessGuard created");
    }
    auto operator->() -> Resource* { return &(monitor_.resource_); }
    auto operator*() -> Resource& { return monitor_.resource_; }

    ~AccessGuard() { std::println("AccessGuard destroyed"); }

   private:
    Monitor& monitor_;
    std::lock_guard<std::mutex> resourceLock_;
  };

 public:
  explicit Monitor(Resource&& resource) : resource_(std::forward<Resource>(resource)) {}

  auto getAccess() -> AccessGuard { return AccessGuard{*this}; }
  auto operator->() -> AccessGuard { return AccessGuard{*this}; }

 private:
  mutable std::mutex mutex_;
  Resource resource_;
};

class MyResource {
 public:
  explicit MyResource(int value) : value_{value} {}
  auto getValue() const -> int { return value_; }
  auto setValue(int value) -> void { value_ = value; }

 private:
  int value_;
};

int main() {
  using std::chrono_literals::operator""s;

  auto myResourceMonitor = Monitor{MyResource{14}};

  auto writeThread = std::jthread([&monitor = myResourceMonitor] {
    auto resource = monitor.getAccess();
    std::println("MyResource value before: {}", resource->getValue());
    resource->setValue(42);
    std::println("MyResource value after: {}", resource->getValue());
  });

  auto readThread = std::jthread([&monitor = myResourceMonitor] {
    auto monitorValue = monitor->getValue();
    std::println("MyResource value in another thread: {}", monitorValue);
  });

  auto delayedReadThread = std::jthread([&monitor = myResourceMonitor] {
    std::this_thread::sleep_for(2s);
    auto monitorValue = monitor->getValue();
    std::println("MyResource value in another thread: {}", monitorValue);
  });
}
// Listing 7.2 Basic Monitor pattern with wrapped integer resource