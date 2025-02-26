#include <print>
#include <vector>
#include <algorithm>
#include <execution>

int main() {
    const auto collection = std::vector<std::string>(1000, "c"); // create a vector of 10 elements, each element is "c"

    // non-parallel transform, add "++" to each element
    auto transform_seq = std::vector<std::string>(collection.size());
    std::transform(collection.cbegin(), collection.cend(), transform_seq.begin(), [](auto& x){ return x + "++"; });
    std::print("Sequential transform successful: {}\n", 
        std::all_of(transform_seq.cbegin(), transform_seq.cend(), [](auto& x){ return x == "c++"; }));

    // parallel transform, add "++" to each element
    auto transform_par = std::vector<std::string>(collection.size());
    std::transform(std::execution::par, collection.cbegin(), collection.cend(), transform_par.begin(), [](auto& x){ return x + "++"; });
    std::print("Parallel transform successful: {}\n", 
        std::all_of(transform_par.cbegin(), transform_par.cend(), [](auto& x){ return x == "c++"; }));

    return 0;
}
// Listing 24: non-parallel and parallel transform