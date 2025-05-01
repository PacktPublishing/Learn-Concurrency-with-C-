#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <print>
#include <optional>

using std::chrono_literals::operator""ms;

template <typename ItemType>
class ConcurrentQueue {
public:
  ConcurrentQueue(std::size_t maxSize = 10) : maxSize_{maxSize} {}

  auto push(const ItemType& value) -> bool {
    {
      auto lock = std::unique_lock{m_};
      queueNotFullSignal_.wait(lock, [this]
               { return (q_.size() < maxSize_)
                        or isCancelled_; });
      if (isCancelled_) {
        return false;
      }
      q_.push(value);
    }
    queueNotEmptySignal_.notify_one();
    return true;
  }

  auto pop() -> std::optional<ItemType> {
    auto lock = std::unique_lock{m_};
    queueNotEmptySignal_.wait(lock, [this]
             { return (not q_.empty())
                      or isCancelled_; });
    if (q_.empty() and isCancelled_) {
      return std::nullopt;
    }
    auto item = q_.front();
    q_.pop();
    queueNotFullSignal_.notify_one();
    return item;
  }

  auto cancel() {
    {
      const auto lock = std::lock_guard{m_};
      isCancelled_ = true;
    }
    queueNotEmptySignal_.notify_all();
    queueNotFullSignal_.notify_all();
  }

private:
  std::queue<ItemType> q_;
  std::mutex m_;
  std::condition_variable queueNotEmptySignal_;
  std::condition_variable queueNotFullSignal_;
  bool isCancelled_ = false;
  std::size_t maxSize_;
};

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
  
  auto consumer2 = [&queue] {
    while (true) {
      auto item = queue.pop();
      if (item.has_value()) {
        std::this_thread::sleep_for(100ms);
        std::println("Consumed by #2: {}", *item);
      } else {
        std::println("Queue cancelled");
        break;
      }
    }
  };

  auto t1 = std::jthread(producer);
  auto t2 = std::jthread(consumer);
  auto t3 = std::jthread(consumer2);
  std::this_thread::sleep_for(500ms);
  std::println("Cancelling operations");
  queue.cancel();
  return 0;
}
// Listing 6.3: Bounded concurrent queue with cancellation and maximum size
