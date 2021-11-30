#include "hades/hades.hpp"

class Testing : public hades::Application {
public:
  std::unique_ptr<hades::Spritesheet> ss;
  std::vector<std::vector<std::string>> sprite_names{};

  int log_counter{1};

  void initialize() override {
    ss = gfx->load_spritesheet("example/resources/8squares.png")
      .set_tiled(8, 8)
      .add_sprite("1", 0, 0)
      .add_sprite("2", 1, 0)
      .add_sprite("3", 2, 0)
      .add_sprite("4", 3, 0)
      .add_sprite("5", 0, 1)
      .add_sprite("6", 1, 1)
      .add_sprite("7", 2, 1)
      .add_sprite("8", 3, 1)
      .build();

    sprite_names = std::vector<std::vector<std::string>>(
      window->h() / ss->tile_h(), 
      std::vector<std::string>(
        window->w() / ss->tile_w()
      )
    );
    randomize_tiles();  // initial setup

    timer->every(0.25, [&]{ 
      randomize_tiles();

      debug_log(fmt::format("Ticked {} times", log_counter));
      log_counter++;
    });
  }

  void randomize_tiles() {
    for (int y = 0; y < sprite_names.size(); ++y)
      for (int x = 0; x < sprite_names[y].size(); ++x)
        sprite_names[y][x] = rnd::choose({"1", "2", "3", "4", "5", "6", "7", "8"}); 
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      shutdown();
  }

  void draw() override {
    gfx->clear_color(hades::rgb(0x181818));
    gfx->clear();

    for (int y = 0; y < sprite_names.size(); ++y)
      for (int x = 0; x < sprite_names[y].size(); ++x)
        ss->draw(sprite_names[y][x], x * ss->tile_w(), y * ss->tile_h());
  }
};

int main(int, char *[]) {
  hades::Runner()
    .open<Testing>({
      .title = "Testing",
      .size = {512, 512},
      .monitor = 1,
      .flags = hades::WFlags::centered
    })
    .initgl()
    .start();
}


