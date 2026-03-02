#include <coroutine>
#include <optional>
#include <print>
#include <thread>

struct Task {
  struct promise_type {
    std::optional<bool> finalValue_;

    auto initial_suspend() noexcept {
      return std::suspend_always{};
    }
    auto final_suspend() noexcept {
      return std::suspend_always{};
    }

    auto get_return_object() {
      return Task{
          std::coroutine_handle<promise_type>::from_promise(
              *this)};
    }

    auto return_value(bool finalValue) noexcept {
      finalValue_.emplace(finalValue);
    }
    auto unhandled_exception() noexcept { std::terminate(); }
  };

  using handle_type = std::coroutine_handle<promise_type>;

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

  auto getResult() {
    if (not isDone()) {
      throw std::runtime_error("Coroutine not finished yet");
    }
    if (not coroutineHandle_.promise().finalValue_) {
      throw std::runtime_error(
          "No value was returned (co_return missing?)");
    }
    return *coroutineHandle_.promise().finalValue_;
  }

 private:
  handle_type coroutineHandle_{nullptr};
};

auto longRunningNetworkTask(int pId) {
  std::println("Long-running network task called, pId: {}", pId);
  return std::suspend_always{};
}

auto connect(int pId) -> Task {
  std::println("Coroutine part 1, pId: {}", pId);
  co_await longRunningNetworkTask(pId);
  std::println("Coroutine part 2, pId: {}", pId);
  co_return true;
}

int main() {
  const auto procedureId = 1;
  auto task = connect(procedureId);
  std::println(
      "This is the first log (coroutine not executed yet)");

  std::println("Resume to execute the first part");
  task.resume();
  std::println("Coroutine suspended again");

  try {
    task.getResult();
  } catch (const std::runtime_error& e) {
    std::println("Premature result acquisition, error: {}",
                 e.what());
  }

  while (not task.isDone()) {
    std::println("Resume to execute the rest of the coroutine");
    task.resume();
  }
  std::println("Done, task result: {}", task.getResult());
}
// Listing 8.4 Simple coroutine task with returning value