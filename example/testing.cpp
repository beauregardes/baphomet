#include "testing.hpp"

using namespace std::chrono_literals;

void afunc() {
  fmt::print("Every 1 second!\n");
}

class Foo {
public:
  Foo() = default;

  void afunc() {
    fmt::print("Every 1 second foo!\n");
  }
};

int main(int, char *[]) {
  auto ticker = baphomet::Ticker();
  auto timer = std::make_unique<baphomet::TimerMgr>();

  timer->after("a tag", 2s, [&]{
    fmt::print("After 2 seconds!\n");
  });

  timer->every("tag 2", 1s, &afunc);

  auto f = Foo();
  timer->every("tag 4", 1s, [&f] { f.afunc(); });

  int n = 5;
  timer->until("tag 3", 1s, [&]{
    fmt::print("Ticking 5 times!\n");
    return --n > 0;
  });

  fmt::print("Starting loop\n");
  do {
    ticker.tick();

    timer->update(ticker.dt());
  } while (ticker.total_elapsed_dt() < 20s);
}
