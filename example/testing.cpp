#include "testing.hpp"

using namespace std::chrono_literals;

class Testing : public hades::Application {
public:
  std::queue<std::string> tags{};

  void initialize() override {
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      shutdown();
  }

  void draw() override {
    gfx->clear_color(hades::rgb("black"));
    gfx->clear();

    if (ImGui::Begin("Test")) {
      if (ImGui::Button("Count")) {
        auto tag = timer->script([&](auto &&wait) {
          debug_log("Counting to 10...");
          for (int i = 1; i <= 10; ++i) {
            wait(1s);
            debug_log("{}!", i);
          }
          debug_log("Done counting!");
        });
        tags.emplace(tag);
      }

      if (ImGui::Button("Count 5")) {
        for (int j = 0; j < 5; ++j) {
          auto tag = timer->script([&](auto &&wait) {
            debug_log("Counting to 10...");
            for (int i = 1; i <= 10; ++i) {
              wait(1s);
              debug_log("{}!", i);
            }
            debug_log("Done counting!");
          });
          tags.emplace(tag);
        }
      }

      if (ImGui::Button("Cancel")) {
        if (!tags.empty()) {
          auto tag = tags.front();
          tags.pop();
          timer->cancel(tag);
          debug_log("Cancelled job {}", tag);
        }
      }

      if (ImGui::Button("Cancel All")) {
        timer->cancel_all();
      }
    }
    ImGui::End();
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
    .init_gl()
    .start();
}