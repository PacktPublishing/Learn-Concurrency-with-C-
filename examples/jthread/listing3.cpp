#include <print>
#include <thread>

int main() {
    auto myJoiningThread = std::jthread([](int param) {
        std::print("Hello from thread! Parameter: {}\n", param);
    }, 21);
    return 0;
}
// Listing 3: JThread example