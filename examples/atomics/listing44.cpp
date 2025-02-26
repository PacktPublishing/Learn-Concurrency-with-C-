#include <memory>
#include <future>
#include <vector>
#include <print>

class SharedResource
{
public:
    SharedResource(): value{0} {}
    SharedResource(int value) : value{value} {}

    auto get() const {
        return value;
    }

private:
    int value;
};

std::atomic<std::shared_ptr<SharedResource>> sharedResource = std::make_shared<SharedResource>(0);

auto modifyResource(int i) {
    sharedResource.store(std::make_shared<SharedResource>(i));
}

int main() {
    auto futures = std::vector<std::future<void>>{};
    auto threadsNr = std::thread::hardware_concurrency();

    std::print("Launching {} threads\n", threadsNr);
    for (int i = 0; i < threadsNr; i++)
    {
        futures.emplace_back(std::async(modifyResource, i));
    }

    std::for_each(futures.begin(), futures.end(), [](auto& f) { f.wait(); }); // wait for completion
    std::print("Threads finished, global resource value: {}\n", sharedResource.load()->get());
    return 0;
}
// Listing 44: Modifying shared pointer resource with atomic operations