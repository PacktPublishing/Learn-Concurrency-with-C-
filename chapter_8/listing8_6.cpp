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

    auto initial_suspend() noexcept {
      return std::suspend_always{};
    }

    auto final_suspend() noexcept {
      // symmetric transfer awaiter
      struct FinalAwaiter {
        auto await_ready() noexcept { return false; }
        auto await_suspend(std::coroutine_handle<promise_type>
                               finishedTaskHandle) noexcept {
          auto continuation =
              finishedTaskHandle.promise().continuation_;
          return continuation ? continuation
                              : std::noop_coroutine();
        }
        auto await_resume() noexcept {}
      };
      return FinalAwaiter{};
    }

    auto get_return_object() {
      return Task{
          std::coroutine_handle<promise_type>::from_promise(
              *this)};
    }
    auto return_value(T v) { finalValue_.emplace(std::move(v)); }
    auto unhandled_exception() { std::terminate(); }
  };

  using handle_type = std::coroutine_handle<promise_type>;

  auto await_ready() { return isDone(); }

  // symmetric transfer suspension
  auto await_suspend(
      std::coroutine_handle<> continuation) noexcept {
    coroutineHandle_.promise().continuation_ = continuation;
    return coroutineHandle_.done() ? continuation
                                   : coroutineHandle_;
  }

  auto await_resume() { return getResult(); }

  explicit Task(handle_type h) : coroutineHandle_(h) {}
  ~Task() {
    if (coroutineHandle_) {
      coroutineHandle_.destroy();
    }
  }

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

auto longRunningNetworkTask(int pId) -> Task<bool> {
  std::println("Long-running network task called, pId: {}", pId);
  co_return true;
}

auto connect(int pId) -> Task<bool> {
  std::println("Coroutine part 1, pId: {}", pId);
  co_await longRunningNetworkTask(pId);
  std::println("Coroutine part 2, pId: {}", pId);
  co_return true;
}

auto driver() -> Task<bool> {
  const auto pId = 1;
  auto result = co_await connect(pId);
  std::println("Done, successful: {}", result);
  co_return true;
}

int main() {
  auto driverCoroutine = driver();
  std::println(
      "This is the first log (coroutine not executed yet)");

  std::println("Resume top-level coroutine");
  driverCoroutine.resume();
  std::println("Done, program result: {}",
               driverCoroutine.getResult());
}