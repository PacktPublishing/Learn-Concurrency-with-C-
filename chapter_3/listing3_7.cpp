#include <algorithm>
#include <execution>
#include <numeric>
#include <print>
#include <random>
#include <ranges>
#include <vector>

using DataVector = std::vector<int>;

auto getRandomNumbers(unsigned size) -> DataVector {
  auto randomGenerator = []() {
    auto seed = std::random_device{}();
    auto eng = std::default_random_engine{seed};
    auto dist = std::uniform_int_distribution<int>(1, 100);
    return dist(eng);
  };
  auto numbers = DataVector(size, 0);
  std::generate(begin(numbers), end(numbers), randomGenerator);
  return numbers;
}

int main() {
  const auto isEven = [](const auto& n) { return n % 2 == 0; };
  const auto squareEl = [](const auto& n) { return n * n; };

  auto numbers = getRandomNumbers(20);

  auto [min, max] = 
    std::ranges::minmax(
      std::execution::par, numbers 
        | std::views::filter
            (std::execution::seq, isEven) 
        | std::views::transform
            (std::execution::par_unseq, squareEl));

  std::print("Min: {}, Max: {}\n", min, max);
  return 0;
}
// Listing 3.7: Proposed syntax for ranges with parallel policies