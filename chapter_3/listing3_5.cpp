#include <algorithm>
#include <execution>
#include <mutex>
#include <print>
#include <vector>

using Add = unsigned;

struct InvalidUnsequencedUsage {
  auto modifyOneByOne(Add toAdd) -> void {
    std::for_each(std::execution::par_unseq, sharedNumbers_.begin(),
                  sharedNumbers_.end(), [this, &toAdd](auto& val) {
                    const auto guard = std::lock_guard(m_);
                    val += toAdd;
                  });
  }

  std::mutex m_;
  std::vector<unsigned> sharedNumbers_{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
};

int main() {
  auto invalidUsage = InvalidUnsequencedUsage{};
  for (auto& n : invalidUsage.sharedNumbers_) {
    std::print("{} ", n);
  }
  std::println();
  invalidUsage.modifyOneByOne(Add{20});
  for (auto& n : invalidUsage.sharedNumbers_) {
    std::print("{} ", n);
  }
  std::println();
}
// Listing 3.5: Invalid use of par_unseq with synchronising operation