#include <functional>
#include <future>
#include <print>
#include <string>
#include <thread>

using std::chrono_literals::operator""ms;
using CallbackFunction = std::function<bool(int)>;

auto longRunningNetworkTask(int pId, int stepId) {
  std::println(
      "Long-running network task called, procedure: #{}, step: "
      "#{}",
      pId, stepId);
  return pId != 1;
}

auto connect(int pId, CallbackFunction onFinished) {
  const auto stepId = 1;
  if (not longRunningNetworkTask(pId, stepId)) {
    return false;
  };
  return onFinished(pId + 20);
}

auto authenticate(int pId, int input,
                  CallbackFunction onFinished) {
  const auto stepId = 2;
  if (not longRunningNetworkTask(pId, stepId)) {
    return false;
  };
  return onFinished(input + 21);
}

auto processData(int pId, int input,
                 CallbackFunction onFinished) {
  const auto stepId = 3;
  if (not longRunningNetworkTask(pId, stepId)) {
    return false;
  };
  return onFinished(input + 22);
}

auto processDataAfterAuthorization(int pId) {
  auto connectResult = connect(pId, [=](int r1) {
    auto authResult = authenticate(pId, r1, [=](int r2) {
      auto processResult = processData(pId, r2, [=](int r3) {
        std::println("Final result : {}", r3);
        return true;
      });
      if (not processResult) {
        std::println("Error in procedure #{} after final action",
                     pId);
      };
      return processResult;
    });
    if (not authResult) {
      std::println("Error in procedure #{} after auth attempt",
                   pId);
    };
    return authResult;
  });
  if (not connectResult) {
    std::println(
        "Error in procedure #{} after connection attempt", pId);
  };
  return connectResult;
}

int main() {
  auto finalAction = [](int res) { return true; };
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
// Listing 8.2 Multi-stage procedure with callbacks and errors