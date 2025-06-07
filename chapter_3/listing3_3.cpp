#include <algorithm>
#include <execution>
#include <print>
#include <vector>

int main() {
  using DataSet = std::vector<int>;
  const auto data = DataSet(1000, 21);

  auto parUnseqPolicyResult = DataSet(data.size());
  parUnseqPolicyResult.reserve(data.size());
  std::transform(std::execution::par_unseq, cbegin(data), cend(data),
                 cbegin(parUnseqPolicyResult),
                 [](const auto& x) { return x + 21; });

  std::println("Parallel-unsequenced transform successful: {}",
               std::all_of(std::execution::par_unseq,
                           cbegin(parUnseqPolicyResult),
                           cend(parUnseqPolicyResult),
                           [](const auto& x) { return x == 42; }));
  return 0;
}
// Listing 3.3: Parallel-unsequenced transform