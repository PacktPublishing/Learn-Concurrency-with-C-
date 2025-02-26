#include <print>
#include <thread>

auto heavyComputation() {
    std::print("Hello from thread!\n");
}

int main() {
    auto myThread = std::thread(heavyComputation);
    myThread.join(); // Wait for the thread to finish
    return 0;
}
// Listing 1: Simple thread creation