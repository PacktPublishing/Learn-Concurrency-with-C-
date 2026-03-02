#include <algorithm>
#include <coroutine>
#include <optional>
#include <print>
#include <utility>
#include <vector>

template <typename T>
struct Task {
  struct promise_type {
    std::optional<T> finalValue_;
    std::coroutine_handle<> continuation_ = nullptr;

    struct FinalAwaiter {
      auto await_ready() noexcept { return false; }
      auto await_suspend(
          std::coroutine_handle<promise_type> h) noexcept {
        auto cont = h.promise().continuation_;
        return cont ? cont : std::noop_coroutine();
      }
      auto await_resume() noexcept {}
    };

    auto initial_suspend() { return std::suspend_always{}; }
    auto final_suspend() noexcept { return FinalAwaiter{}; }

    auto get_return_object() {
      return Task{
          std::coroutine_handle<promise_type>::from_promise(
              *this)};
    }
    auto return_value(T v) { finalValue_.emplace(std::move(v)); }
    auto unhandled_exception() { std::terminate(); }
  };

  using handle_type = std::coroutine_handle<promise_type>;

  auto await_ready() { return false; }
  auto await_suspend(
      std::coroutine_handle<> continuation) noexcept {
    coroutineHandle_.promise().continuation_ = continuation;
    return coroutineHandle_;
  }
  auto await_resume() {
    auto& p = coroutineHandle_.promise();
    if (not p.finalValue_.has_value()) {
      throw std::runtime_error("no value!");
    }
    return p.finalValue_.value();
  }

  explicit Task(handle_type h) : coroutineHandle_(h) {}
  ~Task() {
    if (coroutineHandle_) {
      coroutineHandle_.destroy();
    }
  }

  Task(Task&& other) noexcept
      : coroutineHandle_(
            std::exchange(other.coroutineHandle_, nullptr)) {}
  Task& operator=(Task&& other) noexcept {
    if (this != &other) {
      if (coroutineHandle_) {
        coroutineHandle_.destroy();
      }
      coroutineHandle_ =
          std::exchange(other.coroutineHandle_, nullptr);
    }
    return *this;
  }
  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  auto resume() {
    if (coroutineHandle_ and not coroutineHandle_.done()) {
      coroutineHandle_.resume();
    }
  }

  auto isDone() const {
    return not coroutineHandle_ or coroutineHandle_.done();
  }

  auto getResult() -> T {
    if (not isDone()) {
      throw std::runtime_error("Coroutine not finished yet");
    }
    if (not coroutineHandle_.promise().finalValue_) {
      throw std::runtime_error(
          "No value was returned (co_return missing?)");
    }
    return coroutineHandle_.promise().finalValue_.value();
  }

 private:
  handle_type coroutineHandle_{nullptr};
};

auto longRunningNetworkTask(int pId, int stepId) -> Task<bool> {
  std::println(
      "Long-running network task called, procedure: #{}, step: "
      "#{}",
      pId, stepId);
  co_return pId != 1;
}

auto connect(int pId) -> Task<int> {
  const auto stepId = 1;
  std::println("connect part 1, pId: {}", pId);
  auto opResult = co_await longRunningNetworkTask(pId, stepId);
  std::println("connect part 2, pId: {}", pId);
  if (not opResult) {
    co_return -1;
  }
  co_return pId + 20;
}

auto authenticate(int pId, int input) -> Task<int> {
  const auto stepId = 2;
  std::println("authenticate part 1, pId: {}", pId);
  auto opResult = co_await longRunningNetworkTask(pId, stepId);
  std::println("authenticate part 2, pId: {}", pId);
  if (not opResult) {
    co_return -1;
  }
  co_return input + 21;
}

auto processData(int pId, int input) -> Task<int> {
  const auto stepId = 3;
  std::println("processData part 1, pId: {}", pId);
  auto opResult = co_await longRunningNetworkTask(pId, stepId);
  std::println("processData part 2, pId: {}", pId);
  if (not opResult) {
    co_return -1;
  }
  co_return input + 22;
}

auto processDataAfterAuthorization(int pId) -> Task<bool> {
  auto connectResult = co_await connect(pId);
  if (connectResult == -1) {
    std::println(
        "Error in procedure #{} after connection attempt", pId);
    co_return false;
  }

  auto authResult = co_await authenticate(pId, connectResult);
  if (authResult == -1) {
    std::println("Error in procedure #{} after auth attempt",
                 pId);
    co_return false;
  }

  auto processResult = co_await processData(pId, authResult);
  if (processResult == -1) {
    std::println(
        "Error in procedure #{} after processing attempt", pId);
    co_return false;
  }

  std::println("Final result : {}", processResult);
  co_return true;
}

auto driver(int nrOfTasks) -> Task<std::vector<bool>> {
  auto results = std::vector<bool>{};
  for (auto i = 0; i < nrOfTasks; ++i) {
    auto res = co_await processDataAfterAuthorization(i);
    results.push_back(res);
  }
  co_return results;
}

int main() {
  const auto nrOfTasks = 4;
  auto driverCoroutine = driver(nrOfTasks);
  std::println(
      "This is the first log (coroutine not executed yet)");

  std::println("Resume top-level coroutine");
  driverCoroutine.resume();
  auto res = driverCoroutine.getResult();
  std::println("Done, all tasks successful: {}",
               std::ranges::all_of(
                   res, [](const auto& el) { return el; }));
}