#include <thread>
#include <print>
#include <mutex>

int main() {

    std::mutex m1; // resource nr 1
    std::mutex m2; // resource nr 2

    auto t1 = std::jthread([&m1, &m2]() {
        std::print("Acquiring resource nr 1!\n");
        std::lock_guard<std::mutex> lg1(m1);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::print("Acquiring resource nr 2!\n");
        std::lock_guard<std::mutex> lg2(m2);
    });

    auto t2 = std::jthread([&m1, &m2]() {
        std::print("Acquiring resource nr 2!\n");
        std::lock_guard<std::mutex> lg2(m2); // lock second resource
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::print("Acquiring resource nr 1!\n");
        std::lock_guard<std::mutex> lg1(m1); // lock first resource
    });
    
    return 0;
}
// Listing 34: threads contending for same resources
