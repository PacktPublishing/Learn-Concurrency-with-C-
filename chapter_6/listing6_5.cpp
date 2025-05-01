#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <print>
#include <optional>
#include <expected>

using std::chrono_literals::operator""ms;

enum class QueueOperationStatus {
  Success = 0,
  Cancelled,
  Full,
  Empty
};

auto toString(QueueOperationStatus status) {
   switch(status)
   {
      case QueueOperationStatus::Success:
        return "Success";
      case QueueOperationStatus::Cancelled:
        return "Cancelled";
      case QueueOperationStatus::Full:
        return "Full";
      case QueueOperationStatus::Empty:
        return "Empty";
      default:
        return "unimplemented";
   }
}

template <typename ItemType>
class ConcurrentQueue {
public:
  ConcurrentQueue(std::size_t maxSize = 10) : maxSize_{maxSize} {}

  auto push(const ItemType& value) -> QueueOperationStatus {
    {
      auto lock = std::unique_lock{m_};
      queueNotFullSignal_.wait(lock, [this]
               { return (q_.size() < maxSize_)
                        or isCancelled_; });
      if (isCancelled_) {
        return QueueOperationStatus::Cancelled;
      }
      q_.push(value);
    }
    queueNotEmptySignal_.notify_one();
    return QueueOperationStatus::Success;
  }

  auto pop() -> std::expected<ItemType, QueueOperationStatus> {
    auto lock = std::unique_lock{m_};
    queueNotEmptySignal_.wait(lock, [this]
             { return (not q_.empty())
                      or isCancelled_; });
    if (q_.empty() and isCancelled_) {
      return std::unexpected(QueueOperationStatus::Cancelled);
    }
    auto item = q_.front();
    q_.pop();
    queueNotFullSignal_.notify_one();
    return item;
  }

  auto try_push(const ItemType& value) -> QueueOperationStatus {
    auto lock = std::unique_lock{m_};
    if (q_.size() >= maxSize_) {
      return QueueOperationStatus::Full;
    } else if (isCancelled_) {
      return QueueOperationStatus::Cancelled;
    }
    q_.push(value);
    queueNotEmptySignal_.notify_one();
    return QueueOperationStatus::Success;
  }

  auto try_pop() -> std::expected<ItemType, QueueOperationStatus> {
    auto lock = std::unique_lock{m_};
    if (q_.empty()) {
      return std::unexpected(QueueOperationStatus::Empty);
    } else if (isCancelled_) {
      return std::unexpected(QueueOperationStatus::Cancelled);
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

using ExpectedPopResult = std::expected<int, QueueOperationStatus>;

auto printValue(int value) -> ExpectedPopResult {
  std::this_thread::sleep_for(100ms);
  std::println("Processed: {}", value);
  return ExpectedPopResult(value);
}

auto printFailStatus(QueueOperationStatus value) -> ExpectedPopResult {
  std::this_thread::sleep_for(100ms);
  std::println("Unable to process, queue status: {}", toString(value));
  return ExpectedPopResult(std::unexpected(value));
}

int main() {
  auto queue = ConcurrentQueue<int>{};
  auto producer = [&queue] {
    const auto nrOfItems = 20;
    for (int i = 0; i < nrOfItems; ++i) {
      auto result = queue.push(i);
      if (result == QueueOperationStatus::Success) {
        std::println("Pushed: {}", i);
      } else {
        std::println("Queue cancelled");
        break;
      }
    }
  };

  // producer giving up after 5 tries
  auto producer2 = [&queue] {
    auto nrOfRetries = 5;
    while (nrOfRetries > 0) {
      auto result = queue.try_push(21);
      if (result == QueueOperationStatus::Success) {
        std::println("Pushed: 21");
      } else if (result == QueueOperationStatus::Cancelled) {
        std::println("Queue cancelled");
        break;
      } else {
        std::println("Queue full");
        std::this_thread::sleep_for(100ms);
        --nrOfRetries;
      }
    }
  };

  // consumer working until cancellation
  auto consumer = [&queue] {
    while (true) {
      auto item = queue.pop();
      item.and_then(printValue)
          .or_else(printFailStatus);
      if (item.error_or(QueueOperationStatus::Success) 
          == QueueOperationStatus::Cancelled) {
        std::println("Consume fail, queue cancelled");
        break;
      }
    }
  };
  
  // consumer giving up after 5 tries
  auto consumer2 = [&queue] {
    auto nrOfRetries = 5;
    while (nrOfRetries > 0) {
      auto item = queue.try_pop();
      item.and_then(printValue)
          .or_else(printFailStatus);
      if (item.error_or(QueueOperationStatus::Success) 
          != QueueOperationStatus::Success) {
        std::println("Consume fail, decrementing");
        --nrOfRetries;
      }
    }
  };

  auto t1 = std::jthread(producer);
  auto t2 = std::jthread(producer2);
  auto t3 = std::jthread(consumer);
  auto t4 = std::jthread(consumer2);
  std::this_thread::sleep_for(5500ms);
  std::println("Cancelling operations");
  queue.cancel();
  return 0;
}
// Listing 6.5: Bounded concurrent queue with descriptive operation statuses