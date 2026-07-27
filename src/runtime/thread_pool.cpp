#include "workflow/runtime/thread_pool.hpp"

#include <stdexcept>

namespace workflow {

ThreadPool::ThreadPool(std::size_t workerCount) {
  if (workerCount == 0) {
    throw std::invalid_argument("worker count must be positive");
  }
  workers_.reserve(workerCount);
  for (std::size_t index = 0; index < workerCount; ++index) {
    workers_.emplace_back([this] { workerLoop(); });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::lock_guard lock(mutex_);
    stopping_ = true;
  }
  available_.notify_all();
  for (auto& worker : workers_) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

void ThreadPool::workerLoop() {
  for (;;) {
    std::function<void()> job;
    {
      std::unique_lock lock(mutex_);
      available_.wait(lock, [this] { return stopping_ || !jobs_.empty(); });
      if (stopping_ && jobs_.empty()) {
        return;
      }
      job = std::move(jobs_.front());
      jobs_.pop();
    }
    job();
  }
}

}  // namespace workflow
