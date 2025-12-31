#include <future>
#include <mutex>
#include <print>
#include <vector>

struct SwitchPlaces {
  std::mutex m1;
  int place1{21};

  std::mutex m2;
  int place2{42};
};

SwitchPlaces switchPlaces;

auto modifyResource() {
  auto lock = std::scoped_lock(switchPlaces.m1, switchPlaces.m2);
  std::print("All mutexes locked, place1: {}, place2: {}\n",
             switchPlaces.place1, switchPlaces.place2);
  std::swap(switchPlaces.place1, switchPlaces.place2);
  std::print("Switch done, place1: {}, place2: {}\n",
             switchPlaces.place1, switchPlaces.place2);
}

int main() {
  auto futures = std::vector<std::future<void>>{};
  auto threadsNr = std::thread::hardware_concurrency();

  std::print("Launching {} threads\n", threadsNr);
  for (int i = 0; i < threadsNr; i++) {
    std::print("Launching thread\n");
    futures.emplace_back(std::async(modifyResource));
  }

  std::for_each(futures.begin(), futures.end(),
                [](auto& f) { f.wait(); });
  std::print("Threads finished\n");
  return 0;
}
// Listing 4.8: Locking multiple mutexes using scoped lock