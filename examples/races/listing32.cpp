#include <print>
#include <thread>
#include <vector>

class Resource
{
public:
    auto getX() const { return x; }
    auto setX(int val) { x = val; }

private:
    int x{21};
};

Resource resource{};

auto heavyComputation()
{
    auto x_before = resource.getX(); //obtain resource 
    std::print("Value of x before: {}\n", x_before); 
    auto new_x = x_before + 1; // prepare new value 
    resource.setX(new_x); // store new value 
    auto x_after = resource.getX();; // obtain resource after modification 
    std::print("Value of x after: {}\n", x_after); 
}

auto doWork(int maxThreads)
{
    auto jthreads = std::vector<std::jthread>();
    for (int i = 0; i < maxThreads; i++)
    {
        jthreads.emplace_back(heavyComputation);
    }
}

int main()
{
    auto nrOfThreads = std::thread::hardware_concurrency();
    std::print("Number of threads: {}\n", nrOfThreads);
    doWork(nrOfThreads);
    std::print("Final value of x: {}\n", resource.getX());
    return 0;
}
// Listing 32: Increment x in multiple threads