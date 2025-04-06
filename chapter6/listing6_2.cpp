#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <print>
#include <optional>

template <typename ItemType>
class ConcurrentQueue {
public:
  auto push(const ItemType& value) -> bool {
    {
      const auto lock = std::lock_guard{m_};
      if (isCancelled_) {
        return false;
      }
      q_.push(value);
    }
    cv_.notify_one();
    return true;
  }

  auto pop() -> std::optional<ItemType> {
    auto lock = std::unique_lock{m_};
    cv_.wait(lock, [this]
             { return not q_.empty() or isCancelled_; });
    if (q_.empty() and isCancelled_) {
      return std::nullopt;
    }
    auto item = q_.front();
    q_.pop();
    return item;
  }

  auto cancel() {
    {
      const auto lock = std::lock_guard{m_};
      isCancelled_ = true;
    }
    cv_.notify_all();
  }

private:
  std::queue<ItemType> q_;
  std::mutex m_;
  std::condition_variable cv_;
  bool isCancelled_ = false;
};

using std::chrono_literals::operator""ms;

int main() {
  auto queue = ConcurrentQueue<int>{};
  auto producer = [&queue] {
    const auto nrOfItems = 20;
    for (int i = 0; i < nrOfItems; ++i) {
      auto result = queue.push(i);
      if (result) {
        std::println("Pushed: {}", i);
      } else {
        std::println("Queue cancelled");
        break;
      }
    }
  };

  auto consumer = [&queue] {
    while (true) {
      auto item = queue.pop();
      if (item.has_value()) {
        std::this_thread::sleep_for(100ms);
        std::println("Consumed by #1: {}", *item);
      } else {
        std::println("Queue cancelled");
        break;
      }
    }
  };

  auto t1 = std::jthread(producer);
  auto t2 = std::jthread(consumer);
  std::this_thread::sleep_for(300ms);
  std::println("Cancelling operations");
  queue.cancel();
  return 0;
}
// Listing 6.2: Concurrent queue with cancellation
