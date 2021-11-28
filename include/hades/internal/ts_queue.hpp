#ifndef HADES_TS_QUEUE_HPP
#define HADES_TS_QUEUE_HPP

#include <mutex>
#include <queue>

template<typename T>
class TSQueue {
public:
  void push(T v);
  T pop();
  bool empty();

  void ts_push(T v);
  T ts_pop();
  bool ts_empty();

  void lock();
  void unlock();

private:
  std::queue<T> queue_{};
  std::mutex queue_mutex_;
};

template<typename T>
void TSQueue<T>::push(T v) {
  queue_.push(v);
}

template<typename T>
T TSQueue<T>::pop() {
  auto v = queue_.front();
  queue_.pop();
  return v;
}

template<typename T>
bool TSQueue<T>::empty() {
  return queue_.empty();
}

template<typename T>
void TSQueue<T>::ts_push(T v) {
  const std::lock_guard<std::mutex> lock(queue_mutex_);
  queue_.push(v);
}

template<typename T>
T TSQueue<T>::ts_pop() {
  const std::lock_guard<std::mutex> lock(queue_mutex_);
  auto v = queue_.front();
  queue_.pop();
  return v;
}

template<typename T>
bool TSQueue<T>::ts_empty() {
  const std::lock_guard<std::mutex> lock(queue_mutex_);
  return queue_.empty();
}

template<typename T>
void TSQueue<T>::lock() {
  queue_mutex_.lock();
}

template<typename T>
void TSQueue<T>::unlock() {
  queue_mutex_.unlock();
}

#endif //HADES_TS_QUEUE_HPP
