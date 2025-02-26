#include <memory>
#include <future>
#include <vector>
#include <print>

class SharedResource
{
public:
    SharedResource(): value{0} {}
    SharedResource(int value) : value{value} {}

    auto print() const {
        std::print("SharedResource value: {}\n", value);
    }

    auto increment() {
        ++value;
    }

    auto get() const {
        return value;
    }

private:
    int value{0};
};

std::atomic<std::shared_ptr<SharedResource>> sharedResource = std::make_shared<SharedResource>(0);

auto modifyResource_bad() {
    sharedResource.load()->increment(); // unsafe!!
    sharedResource.load()->print();
}

int main() {
    auto futures = std::vector<std::future<void>>{};
    auto threadsNr = std::thread::hardware_concurrency();

    std::print("Launching {} threads\n", threadsNr);
    for (int i = 0; i < threadsNr; i++)
    {
        futures.emplace_back(std::async(modifyResource_bad));
    }

    std::for_each(futures.begin(), futures.end(), [](auto& f) { f.wait(); }); // wait for completion
    std::print("Threads finished, global resource value: {}\n", sharedResource.load()->get());
    return 0;
}
// Listing 45: Modifying shared pointer resource unsafely