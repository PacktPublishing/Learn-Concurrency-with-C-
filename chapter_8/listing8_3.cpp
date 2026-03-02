#include <coroutine>
#include <print>

struct Task {
  struct promise_type {
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

    auto return_void() noexcept {}
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

 private:
  handle_type coroutineHandle_{nullptr};
};

auto longRunningNetworkTask() {
  std::println("Long-running network task called");
  return std::suspend_always{};
}

auto connect() -> Task {
  std::println("Coroutine part 1");
  co_await longRunningNetworkTask();
  std::println("Coroutine part 2");
}

int main() {
  auto task = connect();
  std::println(
      "This is the first log (coroutine not executed yet)");

  std::println("Resume to execute the first part");
  task.resume();
  std::println("Coroutine suspended again");

  while (not task.isDone()) {
    std::println("Resume to execute the rest of the coroutine");
    task.resume();
  }
  std::println("Done");
}
// Listing 8.3 Simple coroutine task with suspensions