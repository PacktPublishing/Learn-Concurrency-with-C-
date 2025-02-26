#include <print>
#include <thread>

int main() {
    auto lambda = [](int param) {
        std::print("Hello from thread! Parameter: {}\n", param);
    };
    auto myThread = std::thread(lambda, 21);
    myThread.join(); // Wait for the thread to finish
    return 0;
}
// Listing 2: Another way to create a thread