#ifndef HADES_EXAMPLE_TESTING_HPP
#define HADES_EXAMPLE_TESTING_HPP

#include "hades/util/random.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

typedef std::function<void(std::function<void(std::chrono::duration<double>)>)> Job;

struct JobCancelledException : public std::exception {};

class ThreadPool {
public:
  ThreadPool();
  ThreadPool(std::size_t pool_size);

  ~ThreadPool();

  ThreadPool(const ThreadPool &other) = delete;
  ThreadPool &operator=(const ThreadPool &other) = delete;

  ThreadPool(ThreadPool &&other) = delete;
  ThreadPool &operator=(ThreadPool &&other) = delete;

  template<typename T>
  std::string add_job(T &&f);

  template<typename T>
  std::string add_job(const std::string &tag, T &&f);

  void cancel_job(const std::string &tag);

  void shutdown();

private:
  std::queue<std::pair<std::string, Job>> jobs_{};

  std::mutex queue_mutex_;
  std::condition_variable take_a_job_;

  std::mutex wait_mapping_mutex_;
  std::unordered_map<std::string, std::size_t> wait_mapping_;

  std::vector<std::mutex> cancel_mutexes_;
  std::vector<std::condition_variable> cancel_cvs_;
  std::vector<bool> should_cancel_;

  bool terminate_pool_{false};

  std::size_t thread_count_{0};
  std::vector<std::thread> threads_{};

  void initialize_pool_();

  void process_jobs_(std::size_t i);
};

ThreadPool::ThreadPool()
    : ThreadPool(std::thread::hardware_concurrency() - 1) {}

ThreadPool::ThreadPool(std::size_t pool_size)
    : thread_count_(pool_size) {
  initialize_pool_();
}

ThreadPool::~ThreadPool() {
  if (!threads_.empty())
    shutdown();
}

template<typename T>
std::string ThreadPool::add_job(T &&f) {
  return add_job(rnd::base58(11), f);
}

template<typename T>
std::string ThreadPool::add_job(const std::string &tag, T &&f) {
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    jobs_.emplace(tag, f);
  }
  take_a_job_.notify_one();

  return tag;
}

void ThreadPool::cancel_job(const std::string &tag) {
  {
    std::unique_lock<std::mutex> lock(wait_mapping_mutex_);
    auto it = wait_mapping_.find(tag);
    if (it != wait_mapping_.end()) {
      should_cancel_[it->second] = true;
      cancel_cvs_[it->second].notify_all();
    }
  }
}

void ThreadPool::shutdown() {
  terminate_pool_ = true;
  take_a_job_.notify_all();

  for (auto &t : threads_)
    t.join();

  threads_.clear();
}

void ThreadPool::initialize_pool_() {
  for (std::size_t i = 0; i < thread_count_; ++i)
    threads_.emplace_back(&ThreadPool::process_jobs_, this, i);

  cancel_mutexes_ = std::vector<std::mutex>(thread_count_);
  cancel_cvs_ = std::vector<std::condition_variable>(thread_count_);
  should_cancel_ = std::vector<bool>(thread_count_);
}

void ThreadPool::process_jobs_(std::size_t i) {
  while (true) {
    std::pair<std::string, Job> j;
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);

      take_a_job_.wait(lock, [this](){
        return !jobs_.empty() || terminate_pool_;
      });
      if (terminate_pool_)
        break;

      j = jobs_.front();
      jobs_.pop();
    }
    {
      std::unique_lock<std::mutex> lock(wait_mapping_mutex_);
      if (wait_mapping_.contains(j.first)) {
        auto curr_i = wait_mapping_[j.first];
        should_cancel_[curr_i] = true;
        cancel_cvs_[curr_i].notify_all();
      }
      wait_mapping_[j.first] = i;
      should_cancel_[i] = false;
    }

    // FIXME: I'm not a huge fan of using exceptions in any of this code
    //   because generally exceptions are "slow", but I genuinely can't think
    //   of any better way to do this. I'd like to not impose the user having
    //   to put an explicit return in the job functions that are being passed.
    try {
      j.second([&](std::chrono::duration<double> wait_time) {
        std::unique_lock<std::mutex> lock(cancel_mutexes_[i]);
        cancel_cvs_[i].wait_for(lock, wait_time, [&] {
          return should_cancel_[i];
        });
        if (should_cancel_[i])
          throw JobCancelledException();
      });
    } catch (JobCancelledException &) { /* do nothing */ }

    {
      std::unique_lock<std::mutex> lock(wait_mapping_mutex_);
      auto it = wait_mapping_.find(j.first);
      if (it->second == i) {
        should_cancel_[it->second] = false;
        wait_mapping_.erase(it);
      }
    }
  }
}

#endif //HADES_EXAMPLE_TESTING_HPP
