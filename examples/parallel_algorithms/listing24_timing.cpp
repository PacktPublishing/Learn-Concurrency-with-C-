#include <print>
#include <vector>
#include <algorithm>
#include <execution>
#include <numeric>
#include <chrono>

int main() {
    const auto collection = std::vector<std::string>(10000000, "c"); // create a vector of 10 elements, each element is "c"

    // non-parallel transform, add "++" to each element
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto transform_seq = std::vector<std::string>(collection.size());
        std::transform(collection.cbegin(), collection.cend(), transform_seq.begin(), [](auto& x){ return x + "++"; });
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        std::print("Duration: {}\n", duration.count());
        // for(auto& x : transform_seq)
        // {
        //     std::print("El: {}\n", x);
        // }
    }

    {
        auto startTime = std::chrono::high_resolution_clock::now();
        // parallel transform, add "--" to each element
        auto transform_par = std::vector<std::string>(collection.size());
        std::transform(std::execution::par, collection.cbegin(), collection.cend(), transform_par.begin(), [](auto& x){ return x + "--"; });
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::print("Duration: {}\n", duration.count());
        // for(auto& x : transform_par)
        // {
        //     std::print("El: {}\n", x);
        // }
    }
    return 0;
}