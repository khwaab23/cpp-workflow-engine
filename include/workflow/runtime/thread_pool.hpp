#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace workflow {

class ThreadPool {
 public:
  explicit ThreadPool(std::size_t workerCount);
  ~ThreadPool();
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  template <class Function>
  auto submit(Function&& function)
      -> std::future<std::invoke_result_t<Function>> {
    using Result = std::invoke_result_t<Function>;
    auto job = std::make_shared<std::packaged_task<Result()>>(
        std::forward<Function>(function));
    auto future = job->get_future();
    {
      std::lock_guard lock(mutex_);
      if (stopping_) {
        throw std::runtime_error("cannot submit to a stopped thread pool");
      }
      jobs_.emplace([job] { (*job)(); });
    }
    available_.notify_one();
    return future;
  }

 private:
  void workerLoop();

  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> jobs_;
  std::mutex mutex_;
  std::condition_variable available_;
  bool stopping_{false};
};

}  // namespace workflow
