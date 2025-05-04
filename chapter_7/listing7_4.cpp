#include <condition_variable>
#include <functional>
#include <mutex>
#include <print>
#include <shared_mutex>
#include <thread>
#include <utility>

template <typename Predicate, typename Resource>
concept MonitorPredicate =
    std::invocable<Predicate, const Resource&> and
    std::same_as<std::invoke_result_t<Predicate, const Resource&>, bool>;

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

  template <MonitorPredicate<Resource> Predicate>
  auto wait(Predicate&& predicate) const {
    auto lock = std::shared_lock{mutex_};
    cv_.wait(lock, [pred = std::move(predicate), this] {
      return std::invoke(pred, resource_);
    });
  }

  auto notify_all() const {
    auto lock = std::unique_lock{mutex_};
    cv_.notify_all();
  }

 private:
  mutable std::shared_mutex mutex_;
  mutable std::condition_variable_any cv_;
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
    std::println("MyResource value before write thread: {}",
                 resource->getValue());
    resource->setValue(42);
    std::println("MyResource value after write thread: {}",
                 resource->getValue());
  });

  auto readThread1 =
      std::jthread([&constMonitor = std::as_const(myResourceMonitor)] {
        auto monitorValue = constMonitor->getValue();
        std::println("MyResource value in read thread 1: {}", monitorValue);
      });

  auto readThread2 =
      std::jthread([&constMonitor = std::as_const(myResourceMonitor)] {
        auto monitorValue = constMonitor->getValue();
        std::println("MyResource value in read thread 2: {}", monitorValue);
      });

  auto waitingReadThread1 =
      std::jthread([&constMonitor = std::as_const(myResourceMonitor)] {
        std::println("Waiting read thread started");
        constMonitor.wait([](const MyResource& resource) {
          return resource.getValue() > 14;
        });
        std::println("Resource value after waiting read thread 1: {}",
                     constMonitor->getValue());
      });

  auto waitingReadThread2 =
      std::jthread([&constMonitor = std::as_const(myResourceMonitor)] {
        std::println("Waiting read thread started");
        constMonitor.wait([](const MyResource& resource) {
          return resource.getValue() > 90;
        });
        std::println("Resource value after waiting read thread 2: {}",
                     constMonitor->getValue());
      });

  auto waitingWriteThread = std::jthread([&monitor = myResourceMonitor] {
    std::println("Waiting write thread started");
    monitor.wait(
        [](const MyResource& resource) { return resource.getValue() > 14; });
    monitor->setValue(100);
    monitor.notify_all();
    std::println("Resource value after waiting write thread: {}",
                 std::as_const(monitor)->getValue());
  });
}
// Listing 7.4 Monitor pattern with condition_variable_any