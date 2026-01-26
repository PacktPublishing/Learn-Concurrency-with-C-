#include <chrono>
#include <mutex>
#include <print>
#include <shared_mutex>
#include <thread>
#include <vector>

class SharedAccount {
 public:
  auto getBalance() const {
    const auto sharedGuard = std::shared_lock(m_);
    return balance_;
  }
  auto transferFunds(int amount) {
    auto exclusiveGuard = std::unique_lock(m_);
    balance_ += amount;
  }

 private:
  mutable std::shared_mutex m_;
  unsigned balance_{0};
};

int main() {
  auto stopSource = std::stop_source{};
  auto sharedAccount = SharedAccount{};
  auto modifyingThread = std::jthread{
      [&sharedAccount](std::stop_token stopToken) {
        int i = 0;
        while (not stopToken.stop_requested()) {
          if (i % 2) {
            std::println("Withdrawing funds");
            sharedAccount.transferFunds(-40);
          } else {
            std::println("Adding funds");
            sharedAccount.transferFunds(100);
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
        [&sharedAccount](std::stop_token stopToken) {
          while (not stopToken.stop_requested()) {
            std::println("Balance: {}",
                         sharedAccount.getBalance());
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
          }
        },
        stopSource.get_token());
  }
  std::this_thread::sleep_for(std::chrono::seconds(5));
  stopSource.request_stop();
  std::println("Final balance: {}", sharedAccount.getBalance());
  return 0;
}
// Listing 4.6: Shared locking