#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <print>

template <typename ItemType>
class ConcurrentQueue {
public:
  auto push(const ItemType& value) -> void {
    {
      const auto lock = std::lock_guard{m_};
      q_.push(value);
    }
    cv_.notify_one();
  }

  auto pop() -> ItemType {
    auto lock = std::unique_lock{m_};
    cv_.wait(lock, [this]
             { return !q_.empty(); });
    auto item = q_.front();
    q_.pop();
    return item;
  }

private:
  std::queue<ItemType> q_;
  std::mutex m_;
  std::condition_variable cv_;
};

using std::chrono_literals::operator""ms;

int main() {
  auto queue = ConcurrentQueue<int>{};
  auto producer = [&queue] {
    for (int i = 0; i < 10; ++i) {
      std::this_thread::sleep_for(100ms);
      queue.push(i);
      std::println("Pushed: {}", i);
    }
  };

  auto consumer = [&queue] {
    for (int i = 0; i < 10; ++i) {
      auto item = queue.pop();
      std::this_thread::sleep_for(100ms);
      std::println("Consumed: {}", item);
    }
  };

  auto t1 = std::jthread(producer);
  auto t2 = std::jthread(consumer);
  return 0;
}
// Listing 6.1: Concurrent queue with producer and consumer threads
