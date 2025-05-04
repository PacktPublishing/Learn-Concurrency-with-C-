#include <mutex>
#include <print>
#include <shared_mutex>
#include <thread>
#include <utility>

template <typename Resource>
class Monitor {
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
    std::unique_lock<std::shared_mutex> resourceLock_;
  };

  class ConstAccessGuard {
   public:
    ConstAccessGuard(const Monitor& monitor)
        : monitor_{monitor}, resourceLock_{monitor_.mutex_} {
      std::println("ConstAccessGuard created");
    }
    auto operator->() const -> const Resource* { return &(monitor_.resource_); }
    auto operator*() const -> const Resource& { return monitor_.resource_; }

    ~ConstAccessGuard() { std::println("ConstAccessGuard destroyed"); }

   private:
    const Monitor& monitor_;
    std::shared_lock<std::shared_mutex> resourceLock_;
  };

 public:
  Monitor(Resource&& resource) : resource_(std::forward<Resource>(resource)) {}

  auto getAccess() -> AccessGuard { return AccessGuard{*this}; }
  auto getAccess() const -> ConstAccessGuard { return ConstAccessGuard{*this}; }

  auto operator->() -> AccessGuard { return AccessGuard{*this}; }
  auto operator->() const -> const ConstAccessGuard {
    return ConstAccessGuard{*this};
  }

 private:
  mutable std::shared_mutex mutex_;
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

  auto readThread1 =
      std::jthread([&constMonitor = std::as_const(myResourceMonitor)] {
        auto monitorValue = constMonitor->getValue();
        std::println("MyResource value in another thread: {}", monitorValue);
      });

  auto readThread2 =
      std::jthread([&constMonitor = std::as_const(myResourceMonitor)] {
        auto monitorValue = constMonitor->getValue();
        std::println("MyResource value in another thread: {}", monitorValue);
      });

  auto delayedReadThread =
      std::jthread([&constMonitor = std::as_const(myResourceMonitor)] {
        std::this_thread::sleep_for(2s);
        auto monitorValue = constMonitor->getValue();
        std::println("MyResource value in another thread: {}", monitorValue);
      });
}
// Listing 7.3 Monitor pattern with const access and shared mutex