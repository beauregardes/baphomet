#include "hades/hades.hpp"

#include <mutex>
#include <thread>
#include <vector>

class ThreadPool {
public:
  ThreadPool();

private:
  std::size_t hardware_conc_;
  std::vector<std::thread> threads_{};
};

ThreadPool::ThreadPool() {
  hardware_conc_ = std::thread::hardware_concurrency();
}

class Testing : public hades::Application {
public:
  ThreadPool t;

  void initialize() override {
    t = ThreadPool();
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      shutdown();
  }

  void draw() override {
    gfx->clear_color(hades::rgb("black"));
    gfx->clear();
  }
};

int main(int, char *[]) {
  hades::Runner()
      .open<Testing>({
          .title = "Testing",
          .size = {800, 800},
          .monitor = 1,
          .flags = hades::WFlags::centered
      })
      .initgl()
      .start();
}
