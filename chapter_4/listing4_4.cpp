#include <future>
#include <mutex>
#include <print>
#include <vector>

std::recursive_mutex m;
int globalResource = 0;

using std::chrono_literals::operator""s;

auto modifyResourceRecursively(int recursion_depth) -> void {
  auto lock = std::lock_guard(m);
  if (recursion_depth > 0) {
    std::print("Here we go again: {}\n", recursion_depth);
    modifyResourceRecursively(recursion_depth - 1);
  }
  ++globalResource;
  std::print("Incremented resource: {}\n", globalResource);
}

int main() {
  auto futures = std::vector<std::future<void>>{};
  auto threadsNr = std::thread::hardware_concurrency();

  std::print("Launching {} threads, global resource value: {}\n",
             threadsNr, globalResource);
  for (int i = 0; i < threadsNr; i++) {
    futures.emplace_back(
        std::async(modifyResourceRecursively, 3));
  }

  std::for_each(futures.begin(), futures.end(),
                [](auto& f) { f.wait(); });
  std::print("Threads finished, global resource value: {}\n",
             globalResource);
  return 0;
}
// Listing 4.4: Recursive mutex