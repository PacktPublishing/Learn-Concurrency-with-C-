#include <print>
#include <thread>
#include <future>
#include <chrono>

using std::chrono_literals::operator""s;

auto heavyComputation(std::stop_token stopToken, std::promise<int> promise)
{
    try
    {
        throw std::runtime_error("Computation error");
    }
    catch(const std::exception& e)
    {
        promise.set_exception_at_thread_exit(std::current_exception()); // store exception at thread finish
    }

    while(not stopToken.stop_requested())
    {
        std::this_thread::sleep_for(1s); // simulate computation
    }
}

int main() {
    auto promise = std::promise<int>();
    auto future = promise.get_future();
    std::print("Starting work on a detached thread\n");

    auto t = std::jthread(heavyComputation, std::move(promise));
    
    std::print("Is jthread joinable before detach: {}\n", t.joinable());
    t.detach(); // detach t
    std::print("Is jthread joinable after detach: {}\n", t.joinable());

    std::this_thread::sleep_for(1s); 

    std::print("Is stop possible before request: {}\n", t.get_stop_source().stop_possible());
    std::print("Is stop requested: {}\n", t.get_stop_source().stop_requested());
    t.request_stop();
    std::print("Is stop possible after request: {}\n", t.get_stop_source().stop_possible());
    std::print("Is stop requested: {}\n", t.get_stop_source().stop_requested());
    
    std::this_thread::sleep_for(1s); 

    try
    {
        std::print("Obtaining result...\n");
        future.get(); // we know that an exception is gonna be here :)
    }
    catch(const std::exception& e)
    {
        std::print("Something happened: {}\n", e.what());
    }
    return 0;
}
// Listing 21: Starting a detached jthread with a promise