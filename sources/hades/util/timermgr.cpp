#include "hades/util/timermgr.hpp"

namespace hades {

void TimerMgr::initialize_script_pool(std::size_t num_threads) {
  if (!script_pool_)
    script_pool_ = std::make_unique<ThreadPool>(num_threads);
}

void TimerMgr::update(double dt) {
  auto it = timers_.begin();
  while (it != timers_.end()) {
    if (!it->second->paused) {
      it->second->update(dt);

      if (it->second->triggered)
        it->second->fire();

      if (it->second->expired)
        it = timers_.erase(it);
      else
        it++;

    } else
      it++;
  }
}

void TimerMgr::toggle(const std::string &tag) {
  auto it = timers_.find(tag);
  if (it != timers_.end())
    it->second->paused = !it->second->paused;
}

void TimerMgr::pause(const std::string &tag) {
  auto it = timers_.find(tag);
  if (it != timers_.end())
    it->second->paused = true;
}

void TimerMgr::resume(const std::string &tag) {
  auto it = timers_.find(tag);
  if (it != timers_.end())
    it->second->paused = false;
}

void TimerMgr::cancel(const std::string &tag) {
  auto it = timers_.find(tag);
  if (it != timers_.end())
    timers_.erase(it);
  else
    script_pool_->cancel_job(tag);
}

void TimerMgr::cancel_all() {
  timers_.clear();
  script_pool_->cancel_all();
}

void TimerMgr::initialize_script_pool_() {
  script_pool_ = std::make_unique<ThreadPool>();
}

} // namespace hades