#include <print>
#include <vector>
#include <numeric>
#include <ranges>
#include <algorithm>
#include <random>

auto getRandomNumbers(unsigned size) -> std::vector<int>
{
    auto randomGenerator = [](){    
        std::random_device rd;
        std::uniform_int_distribution<int> dist(0, 100);
        return dist(rd);
    };
    auto numbers = std::vector<int>(size, 0);
    std::generate(numbers.begin(), numbers.end(), randomGenerator);
    return numbers;
}

int main() {
    auto numbers = getRandomNumbers(20);

    auto [min, max] = std::ranges::minmax(numbers
            | std::views::filter([](int n) { return n % 2 == 0; })
            | std::views::transform([](int n) { return n * n; }));

    std::print("min: {}, max: {}\n", min, max);
    return 0;
}
// Listing 30: chaining several operations on a set of data using ranges