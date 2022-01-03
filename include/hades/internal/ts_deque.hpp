#ifndef HADES_TS_DEQUE_HPP
#define HADES_TS_DEQUE_HPP

#include <mutex>
#include <deque>

template<typename T>
class TSDeque {
public:
  void push_front(T v);
  void push_back(T v);
  T pop_front();
  T pop_back();
  T &back();
  T &front();
  bool empty();

  void ts_push_front(T v);
  void ts_push_back(T v);
  T ts_pop_front();
  T ts_pop_back();
  bool ts_empty();

  void lock();
  void unlock();

  typename std::deque<T>::iterator begin();
  typename std::deque<T>::iterator end();

private:
  std::deque<T> deque_{};
  std::mutex deque_mutex_;
};

template<typename T>
void TSDeque<T>::push_front(T v) {
  deque_.push_front(v);
}

template<typename T>
void TSDeque<T>::push_back(T v) {
  deque_.push_back(v);
}

template<typename T>
T TSDeque<T>::pop_front() {
  auto v = deque_.front();
  deque_.pop_front();
  return v;
}

template<typename T>
T TSDeque<T>::pop_back() {
  auto v = deque_.back();
  deque_.pop_back();
  return v;
}

template<typename T>
T &TSDeque<T>::back() {
  return deque_.back();
}

template<typename T>
T &TSDeque<T>::front() {
  return deque_.front();
}

template<typename T>
bool TSDeque<T>::empty() {
  return deque_.empty();
}

template<typename T>
void TSDeque<T>::ts_push_front(T v) {
  const std::lock_guard<std::mutex> lock(deque_mutex_);
  deque_.push_front(v);
}

template<typename T>
void TSDeque<T>::ts_push_back(T v) {
  const std::lock_guard<std::mutex> lock(deque_mutex_);
  deque_.push_back(v);
}

template<typename T>
T TSDeque<T>::ts_pop_front() {
  const std::lock_guard<std::mutex> lock(deque_mutex_);
  auto v = deque_.front();
  deque_.pop_front();
  return v;
}

template<typename T>
T TSDeque<T>::ts_pop_back() {
  const std::lock_guard<std::mutex> lock(deque_mutex_);
  auto v = deque_.back();
  deque_.pop_back();
  return v;
}

template<typename T>
bool TSDeque<T>::ts_empty() {
  const std::lock_guard<std::mutex> lock(deque_mutex_);
  return deque_.empty();
}

template<typename T>
void TSDeque<T>::lock() {
  deque_mutex_.lock();
}

template<typename T>
void TSDeque<T>::unlock() {
  deque_mutex_.unlock();
}

template<typename T>
typename std::deque<T>::iterator TSDeque<T>::begin(){
  return deque_.begin();
}

template<typename T>
typename std::deque<T>::iterator TSDeque<T>::end() {
  return deque_.end();
}

#endif //HADES_TS_DEQUE_HPP
