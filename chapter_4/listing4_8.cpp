#include <chrono>
#include <future>
#include <print>
#include <shared_mutex>
#include <thread>
#include <vector>

class SharedAccount {
 public:
  SharedAccount(unsigned initialBalance)
      : balance_{initialBalance} {}

  auto getBalance() const {
    const auto sharedGuard = std::shared_lock(m_);
    return balance_;
  }
  auto transferFunds(int amount) {
    auto exclusiveGuard = std::unique_lock(m_);
    balance_ += amount;
  }

  friend auto transferBetween(SharedAccount& from,
                              SharedAccount& to, int amount)
      -> bool;

  template <typename... Args>
  friend auto getTotalBalance(Args&... accounts) -> unsigned;

 private:
  mutable std::shared_mutex m_;
  unsigned balance_{0};
};

auto transferBetween(SharedAccount& from, SharedAccount& to,
                     int amount) -> bool {
  if (&from == &to) return false;
  auto guard = std::scoped_lock(from.m_, to.m_);
  if (from.balance_ >= amount) {
    from.balance_ -= amount;
    to.balance_ += amount;
  } else {
    return false;
  }
  return true;
}

template <typename... Args>
auto getTotalBalance(Args&... accounts) -> unsigned {
  auto locks = std::tuple{
      std::shared_lock(accounts.m_, std::defer_lock)...};

  std::apply([&](auto&... lks) { std::lock(lks...); }, locks);
  return (accounts.balance_ + ...);
}

int main() {
  auto stopSource = std::stop_source{};
  auto sharedAccount1 = SharedAccount{200};
  auto sharedAccount2 = SharedAccount{200};
  auto modifyingThread = std::jthread{
      [&sharedAccount1,
       &sharedAccount2](std::stop_token stopToken) {
        int i = 0;
        while (not stopToken.stop_requested()) {
          auto result = false;
          if (i % 2) {
            result = transferBetween(sharedAccount1,
                                     sharedAccount2, 40);
          } else {
            result = transferBetween(sharedAccount2,
                                     sharedAccount1, 100);
          }
          if (not result) {
            std::println("Transfer failed!");
          }
          std::this_thread::sleep_for(std::chrono::seconds(1));
          i++;
        }
      },
      stopSource.get_token()};

  auto readingThreads = std::vector<std::jthread>{};
  const auto readingThreadsCount = 20;
  readingThreads.reserve(readingThreadsCount);
  for (auto i = 0; i < readingThreadsCount; ++i) {
    readingThreads.emplace_back(
        [&sharedAccount1,
         &sharedAccount2](std::stop_token stopToken) {
          while (not stopToken.stop_requested()) {
            std::println(
                "Balance: {}",  // in this case should always
                                // remain the same, as we're
                                // transferring between 2
                                // accounts all the time
                getTotalBalance(sharedAccount1, sharedAccount2));
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
          }
        },
        stopSource.get_token());
  }
  std::this_thread::sleep_for(std::chrono::seconds(5));
  stopSource.request_stop();
  return 0;
}
// Listing 4.8: Locking multiple mutexes in shared lock