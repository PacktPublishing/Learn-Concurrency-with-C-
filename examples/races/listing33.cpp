#include <thread>
#include <print>

auto thread_func1(std::thread& t2) {
    std::print("Thread 1 started\n");
    t2.join(); // wait for t2 to finish
    std::print("Thread 1 finished\n");
}

auto thread_func2(std::thread& t1) {
    std::print("Thread 2 started\n");
    t1.join(); // wait for t1 to finish
    std::print("Thread 2 finished\n");
}

int main() {
    std::thread t1;
    std::thread t2;
    t1 = std::thread(thread_func1, std::ref(t2));
    t2 = std::thread(thread_func2, std::ref(t1));

    t1.join();
    t2.join();

    std::print("Main thread finished\n"); // never printed, threads deadlocked
    return 0;
}
// Listing 33: threads waiting for each other before finishing, resulting in a deadlock
