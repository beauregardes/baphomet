#include "hades/hades.hpp"

const auto FONT = "resources/font/phantasm_10x10_no_bg.png";
const int CHAR_W = 10;
const int CHAR_H = 10;

class Testing : public hades::Application {
public:
  std::unique_ptr<hades::CP437> phantasm{nullptr};
  std::string sample_text;

  void initialize() override {
    phantasm = gfx->load_cp437(FONT, CHAR_W, CHAR_H, true);

    for (int y = 0; y < 64; y++) {
      for (int x = 0; x < 64; x++)
        sample_text += "#";
      if (y < 63)
        sample_text += "\n";
    }

    timer->every(1.0/1000.0, [&]{ corrupt_text(); });
  }

  void corrupt_text() {
    int pos;
    do {
      pos = rnd::get<int>(sample_text.size() - 1);
    } while (sample_text[pos] == '\n');
    sample_text[pos] = rnd::get<char>(33, 126);
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      shutdown();
  }

  void draw() override {
    gfx->clear_color(hades::rgb("black"));
    gfx->clear();

    phantasm->render(0, 0, sample_text);
  }
};

int main(int, char *[]) {
  hades::Runner()
    .open<Testing>({
        .title = "Testing",
        .size = {CHAR_W*64, CHAR_H*64},
        .monitor = 1,
        .flags = hades::WFlags::centered
    })
    .initgl()
    .start();
}
