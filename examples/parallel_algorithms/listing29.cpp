#include <print>
#include <vector>
#include <algorithm>
#include <numeric>
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

    auto filteredNumbers = std::vector<int>();
    std::copy_if(numbers.cbegin(), numbers.cend(),
            std::back_inserter(filteredNumbers), 
            [](auto& n){ return n % 2 == 0; });

    auto squaredNumbers = std::vector<int>();
    std::transform(filteredNumbers.cbegin(), filteredNumbers.cend(), 
                   std::back_inserter(squaredNumbers),
                   [](auto& val) { return val * val; });
                   
    auto [min, max] = std::minmax_element(squaredNumbers.cbegin(), squaredNumbers.cend());
    std::print("min: {}, max: {}\n", *min, *max);

    return 0;
}
// Listing 29: performing several operations on a set of data without ranges