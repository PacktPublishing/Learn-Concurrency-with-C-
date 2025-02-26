#include <print>
#include <vector>
#include <algorithm>
#include <mutex>
#include <execution>

using Add = unsigned;

struct InvalidUnsequencedUsage {
  auto modifyOneByOne(Add toAdd) -> void
  {
      std::for_each(std::execution::par_unseq, // parallel unsequenced policy
                    sharedNumbers_.begin(), sharedNumbers_.end(),
                    [this, &toAdd](auto& val) {
                      std::lock_guard guard(m_); // lock before modifying shared state
                      val+=toAdd;
                    });
  }

  std::mutex m_;
  std::vector<int> sharedNumbers_{1,2,3,4,5,6,7,8,9,10};
};

int main() {
    auto invalidUsage = InvalidUnsequencedUsage{};
    for (auto& n : invalidUsage.sharedNumbers_) {
        std::print("{} ", n);
    }
    std::print("\n");
    invalidUsage.modifyOneByOne(Add{20});
    for (auto& n : invalidUsage.sharedNumbers_) {
        std::print("{} ", n);
    }
    std::print("\n");
}
// Listing 28: invalid use of par_unseq with synchronising operation