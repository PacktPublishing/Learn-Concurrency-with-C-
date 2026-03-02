#include <functional>
#include <future>
#include <print>
#include <string>
#include <thread>

using std::chrono_literals::operator""ms;
using CallbackFunction = std::function<void(int)>;

auto longRunningNetworkTask(int pId, int stepId) {
  std::println(
      "Long-running network task called, procedure: #{}, step: "
      "#{}",
      pId, stepId);
}

auto connect(int pId, CallbackFunction onFinished) {
  const auto stepId = 1;
  longRunningNetworkTask(pId, stepId);
  onFinished(pId + 20);
}

auto authenticate(int pId, int input,
                  CallbackFunction onFinished) {
  const auto stepId = 2;
  longRunningNetworkTask(pId, stepId);
  onFinished(input + 21);
}

auto processData(int pId, int input,
                 CallbackFunction onFinished) {
  const auto stepId = 3;
  longRunningNetworkTask(pId, stepId);
  onFinished(input + 22);
}

auto processDataAfterAuthorization(int pId) {
  connect(pId, [=](int r1) {
    authenticate(pId, r1, [=](int r2) {
      processData(pId, r2, [=](int r3) {
        std::println("Final result : {}", r3);
      });
    });
  });
}

int main() {
  std::println("Starting asynchronous execution");
  auto nrOfTasks = 3;
  auto futures = std::vector<std::future<void>>{};
  futures.reserve(nrOfTasks);
  for (auto i = 0; i < nrOfTasks; ++i) {
    futures.emplace_back(std::async(std::launch::async, [=] {
      processDataAfterAuthorization(i);
    }));
  }
  std::println("Waiting for actions fo finish");
  for (auto& f : futures) {
    f.get();
  }
}
// Listing 8.1 Multi-stage procedure with callbacks