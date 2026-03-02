#include <algorithm>
#include <coroutine>
#include <optional>
#include <print>
#include <type_traits>
#include <vector>

struct my_suspend_always {
  constexpr bool await_ready() const noexcept { return false; }
  constexpr void await_suspend(
      std::coroutine_handle<>) const noexcept {}
  constexpr void await_resume() const noexcept {}
};

template <typename T>
struct Task {
  struct promise_type {
    std::optional<T> finalValue_;

    auto initial_suspend() noexcept {
      return my_suspend_always{};
    }
    auto final_suspend() noexcept { return my_suspend_always{}; }

    auto get_return_object() {
      return Task{
          std::coroutine_handle<promise_type>::from_promise(
              *this)};
    }

    auto return_value(T finalValue) noexcept {
      finalValue_.emplace(finalValue);
    }
    auto unhandled_exception() noexcept { std::terminate(); }
  };

  using handle_type = std::coroutine_handle<promise_type>;

  auto await_ready() { return isDone(); }

  // do not actually do this
  auto await_suspend(std::coroutine_handle<> ignored) noexcept {
    while (not isDone()) {
      coroutineHandle_.resume();
    }
    std::println("suspend exit");
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
  std::println("Done 1, successful: {}", result);
  result = co_await connect(pId);
  std::println("Done 2, successful: {}", result);
  co_return true;
}

int main() {
  auto driverCoroutine = driver();
  std::println(
      "This is the first log (coroutine not executed yet)");

  while (not driverCoroutine.isDone()) {
    std::println("Resume top-level coroutine");
    driverCoroutine.resume();
  }
  std::println("Done, program result: {}",
               driverCoroutine.getResult());
}

// void support

// struct promise_type {
//   using ValueType =
//       std::conditional_t<std::is_void_v<T>, std::monostate,
//       T>;
//   std::optional<ValueType> finalValue_;

//   auto initial_suspend() noexcept {
//     std::println("initial!");
//     return my_suspend_always{};
//   }
//   auto final_suspend() noexcept {
//     std::println("final!");
//     return my_suspend_always{};
//   }

//   auto get_return_object() {
//     return Task{
//         std::coroutine_handle<promise_type>::from_promise(
//             *this)};
//   }

//   // === NEW: works for both T and void ===
//   void return_value(T finalValue)
//     requires(!std::is_void_v<T>)
//   {
//     finalValue_.emplace(finalValue);
//   }
//   void return_void()
//     requires(std::is_void_v<T>)
//   {
//     finalValue_.emplace(std::monostate{});
//   }

//   auto unhandled_exception() noexcept { std::terminate(); }
// };