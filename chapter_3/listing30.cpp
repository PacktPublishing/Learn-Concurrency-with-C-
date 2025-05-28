#include <algorithm>
#include <numeric>
#include <print>
#include <random>
#include <ranges>
#include <vector>

using DataSet = std::vector<int>;

auto getRandomNumbers(unsigned size) -> DataSet {
  auto randomGenerator = []() {
    auto seed = std::random_device{}();
    auto eng = std::default_random_engine{seed};
    auto dist = std::uniform_int_distribution<int>(1, 100);
    return dist(eng);
  };
  auto numbers = DataSet(size, 0);
  std::generate(begin(numbers), end(numbers), randomGenerator);
  return numbers;
}

int main() {
  const auto isEven = [](const auto& n) { return n % 2 == 0; };
  const auto squareEl = [](const auto& n) { return n * n; };

  auto numbers = getRandomNumbers(20);

  auto [min, max] =
    std::ranges::minmax(numbers 
                        | std::views::filter(isEven) 
                        | std::views::transform(squareEl));

  std::print("Min: {}, Max: {}\n", min, max);
  return 0;
}
// Listing 30: chaining several operations on a set of data using ranges