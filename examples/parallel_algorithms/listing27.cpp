#include <print>
#include <vector>
#include <algorithm>
#include <execution>

int main() {
    const auto collection = std::vector<int>(1000, 21);
    try
    {
        auto transform = std::vector<int>(collection.size());
        std::transform(collection.cbegin(), collection.cend(), transform.begin(), 
        [](auto&){ 
            // this exception will be caught
            throw std::runtime_error("error"); 
            return 1; 
        });
    }
    catch(...)
    {
        // print will happen
        std::print("error from non-policy transform\n");
    }

    try
    {
        auto transform = std::vector<int>(collection.size());
        std::transform(std::execution::seq, collection.cbegin(), collection.cend(), transform.begin(), 
        [](auto&){ 
            // this exception will not be caught
            throw std::runtime_error("error"); 
            return 1; 
        });
    }
    catch(...)
    {
        // no print will happen
        std::print("error from policy transform\n");
    }

    return 0;
}
// Listing 27: Exceptions in algorithms with execution policies