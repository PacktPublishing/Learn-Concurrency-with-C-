#include <algorithm>
#include <execution>
#include <print>
#include <string>
#include <vector>

int main() {
  using DataSet = std::vector<std::string>;
  const auto data = DataSet(1000, "c");

  const auto isSuccess = [](const auto& collection) {
    return std::all_of(cbegin(collection), cend(collection),
                       [](const auto& x) { return x == "c++"; });
  };
  const auto addSuffix = [](const auto& x) { return x + "++"; };

  auto noPolicyResult = DataSet{};
  noPolicyResult.reserve(data.size());
  std::transform(cbegin(data), cend(data),
                 std::back_inserter(noPolicyResult), addSuffix);
  std::println("Is sequential transform successful: {}",
               isSuccess(noPolicyResult));

  auto parPolicyResult = DataSet{};
  parPolicyResult.reserve(data.size());
  std::transform(std::execution::par, cbegin(data), cend(data),
                 std::back_inserter(parPolicyResult), addSuffix);
  std::println("Is parallel transform successful: {}",
               isSuccess(parPolicyResult));
  return 0;
}
// Listing 24: non-parallel and parallel transform