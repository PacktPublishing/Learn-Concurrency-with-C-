#include <algorithm>
#include <execution>
#include <print>
#include <string>
#include <vector>

int main() {
  using DataSet = std::vector<std::string>;
  const auto data = DataSet(10, "c++");

  auto allElementsEqual = [&data](auto& value) {
    return std::all_of(std::execution::par, cbegin(data), cend(data),
                       [&value](const auto& x) { return x == value; });
  };

  std::println("All elements are 'c++': {}", allElementsEqual("c++"));
  std::println("All elements are 'c--': {}", allElementsEqual("c--"));
  return 0;
}
// Listing 3.2: parallel all_of algorithm