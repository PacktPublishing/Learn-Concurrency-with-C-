#include <print>
#include <vector>
#include <algorithm>
#include <execution>

int main() {
    const auto collection = std::vector<std::string>(10, "c++"); // create a vector of 10 elements, each element is "c"

    auto allElementsEqual = [&collection](auto& value) {
        return std::all_of(std::execution::par, collection.cbegin(), collection.cend(), [&value](const auto& x){ return x == value; });
    };

    std::print("All elements are 'c++': {}\n", allElementsEqual("c++"));
    std::print("All elements are 'c--': {}\n", allElementsEqual("c--"));
    return 0;
}
// Listing 25: parallel all_of algorithm