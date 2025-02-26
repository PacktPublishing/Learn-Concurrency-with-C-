#include <print>
#include <vector>
#include <algorithm>
#include <execution>

int main() {
    const auto collection = std::vector<int>(1000, 21); // create a vector of 10 elements, each element is "c"

    // parallel and vectorized transform, add 21 to each element, making it 42
    auto transform_par = std::vector<int>(collection.size());
    std::transform(std::execution::par_unseq, collection.cbegin(), collection.cend(), transform_par.begin(), [](auto& x){ return x + 21; });
    std::print("Parallel-unsequenced transform successful: {}\n", 
        std::all_of(std::execution::par_unseq, transform_par.cbegin(), transform_par.cend(), [](auto& x){ return x == 42; }));

    return 0;
}
// Listing 26: Parallel-unsequenced transform