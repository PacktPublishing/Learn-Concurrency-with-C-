#include <future>
#include <mutex>
#include <print>
#include <vector>

class ShoppingList {
 public:
  auto addItem(std::string item) {
    const auto guard = std::lock_guard(m_);
    shoppingList_.emplace_back(std::move(item));
  }
  auto getSummary() const {
    const auto guard = std::lock_guard(m_);
    if (shoppingList_.empty()) {
      return std::string{"Shopping List:\n(no items)\n"};
    }
    auto result = std::string{"Shopping List:\n"};
    for (const auto& item : shoppingList_) {
      result += std::format("  • {}\n", item);
    }
    return result;
  }

 private:
  mutable std::mutex m_;
  std::vector<std::string> shoppingList_;
};

int main() {
  auto futures = std::vector<std::future<void>>{};
  auto threadCount = std::thread::hardware_concurrency();
  futures.reserve(threadCount);

  auto sharedList = ShoppingList{};

  std::println("Initial list:\n{}", sharedList.getSummary());
  for (int i = 0; i < threadCount; i++) {
    futures.emplace_back(std::async([&sharedList, i] {
      if (i % 2) {
        sharedList.addItem("Apple");
      } else {
        sharedList.addItem("Banana");
      }
    }));
  }
  std::for_each(futures.begin(), futures.end(),
                [](auto& f) { f.wait(); });
  std::println("Final list:\n{}", sharedList.getSummary());
  return 0;
}
// Listing 4.3: Thread-safe shopping list