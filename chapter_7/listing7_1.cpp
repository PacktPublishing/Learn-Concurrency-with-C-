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

int main() {
  using std::chrono_literals::operator""s;

  auto intMonitor = Monitor{14};

  auto writeThread = std::jthread([&monitor = intMonitor] {
    auto resourceGuard = monitor.getAccess();
    std::println("int resource before: {}", (*resourceGuard));
    *resourceGuard = 42;
    std::println("int resource after: {}", (*resourceGuard));
  });

  auto readThread = std::jthread([&monitor = intMonitor] {
    auto monitorValue = *monitor.getAccess();
    std::println("int resource in another thread: {}", monitorValue);
  });

  auto delayedReadThred = std::jthread([&monitor = intMonitor] {
    std::this_thread::sleep_for(2s);
    auto monitorValue = *monitor.getAccess();
    std::println("int resource in another thread: {}", monitorValue);
  });
}
// Listing 7.1 Basic Monitor pattern with integer resource