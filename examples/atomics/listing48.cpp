#include <print>
#include <thread>
#include <atomic>

std::atomic_bool ready{false};

using std::chrono_literals::operator""ms;

auto trigger() {
    std::this_thread::sleep_for(500ms); // simulate preparation for trigger
    ready.store(21);
    ready.notify_all(); // wake up all workers
}

auto worker(int id) {
    ready.wait(false); // wait until ready != false
    std::print("Worker {} has woken up\n", id);
}

int main() {
    auto w1 = std::jthread(worker, 1);
    auto w2 = std::jthread(worker, 2);
    auto w3 = std::jthread(worker, 3);
    auto t = std::jthread(trigger); // workers prepared, trigger wakeup
    return 0;
}
// Listing 48: worker threads triggered using atomic wait and notify