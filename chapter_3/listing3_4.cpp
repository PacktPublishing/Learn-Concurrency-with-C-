#include <algorithm>
#include <execution>
#include <print>
#include <vector>

int main() {
  using DataSet = std::vector<int>;
  const auto data = DataSet(1000, 21);

  try {
    auto noPolicyResult = DataSet{};
    noPolicyResult.reserve(data.size());
    std::transform(cbegin(data), cend(data),
                   std::back_inserter(noPolicyResult), [](auto&) {
                     throw std::runtime_error("Non-terminating");
                     return 1;
                   });
  } catch (...) {
    std::println("Error from non-policy transform");
  }

  try {
    auto seqPolicyResult = DataSet{};
    seqPolicyResult.reserve(data.size());
    std::transform(std::execution::seq, cbegin(data), cend(data),
                   begin(seqPolicyResult), [](auto&) {
                     throw std::runtime_error("Terminating");
                     return 1;
                   });
  } catch (...) {
    std::println("This won't be printed anyway");
  }
  return 0;
}
// Listing 3.4: Exceptions in algorithms with execution policies