#include <algorithm>
#include <numeric>
#include <print>
#include <random>
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
  const auto getMinMax = [](const auto& collection) {
    auto [min, max] =
        std::minmax_element(cbegin(collection), cend(collection));
    std::print("Min: {}, Max: {}\n", *min, *max);
  };
  const auto isEven = [](const auto& n) { return n % 2 == 0; };
  const auto squareEl = [](const auto& n) { return n * n; };

  auto numbers = getRandomNumbers(20);
  getMinMax(numbers);

  auto filteredNumbers = DataSet{};
  filteredNumbers.reserve(numbers.size());
  std::copy_if(cbegin(numbers), cend(numbers),
               std::back_inserter(filteredNumbers), isEven);
  getMinMax(filteredNumbers);

  auto squaredNumbers = DataSet{};
  squaredNumbers.reserve(numbers.size());
  std::transform(cbegin(filteredNumbers), cend(filteredNumbers),
                 std::back_inserter(squaredNumbers), squareEl);

  getMinMax(squaredNumbers);
  return 0;
}
// Listing 29: performing several operations on a dataset without ranges