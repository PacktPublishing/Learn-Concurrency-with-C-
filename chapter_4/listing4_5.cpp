#include <future>
#include <print>
#include <shared_mutex>
#include <vector>

std::shared_mutex m;
int globalResource = 0;

using std::chrono_literals::operator""s;

auto modifyResource() -> void {
  auto lock = std::unique_lock(m);
  std::print("Modifying resource\n");
  globalResource += 21;
  std::this_thread::sleep_for(1s);
  std::print("Resource modified: {}\n", globalResource);
}

auto readReasource() -> void {
  auto lock = std::shared_lock(m);
  std::print("Reading resource: {}\n", globalResource);
}

int main() {
  auto futures = std::vector<std::future<void>>{};
  auto threadsNr = std::thread::hardware_concurrency();

  std::print("Launching {} threads, global resource value: {}\n",
             threadsNr, globalResource);
  for (int i = 0; i < threadsNr; i++) {
    if (i % (threadsNr / 2)) {
      std::print("Launching reader thread\n");
      futures.emplace_back(std::async(readReasource));
    } else {
      std::print("Launching writer thread\n");
      futures.emplace_back(std::async(modifyResource));
    }
  }

  std::for_each(futures.begin(), futures.end(),
                [](auto& f) { f.wait(); });
  std::print("Threads finished, global resource value: {}\n",
             globalResource);
  return 0;
}
// Listing 4.5: Shared mutex