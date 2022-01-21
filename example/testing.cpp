#include "testing.hpp"

const auto RESOURCES = fs::path(__FILE__).parent_path() / "resources";

class Score {
public:
  Score(const std::string &path) {
    std::ifstream ifs(path);
    if (ifs.is_open()) {
      std::string line;
      while (std::getline(ifs, line)) {
        auto comp = hades::split(line, " | ");
        notes_.emplace_back(
            hades::split(comp[0]),
            std::stod(comp[1])
        );
      }
    } else
      spdlog::error("Failed to open score: '{}'", path);
  }

  std::optional<std::pair<std::vector<std::string>, double>> step() {
    if (curr_note < notes_.size()) {
      return notes_[curr_note++];
    }
    return std::nullopt;
  }

private:
  std::vector<std::pair<std::vector<std::string>, double>> notes_{};
  std::size_t curr_note{0};
};

class Testing : public hades::Application {
public:
  std::vector<std::string> notes =
      {"b3", "c4", "d4", "e4", "f4", "g4", "a4", "b4", "c5", "d5"};
  std::unique_ptr<Score> score{nullptr};

  void initialize() override {
    for (const auto &n : notes)
      audio->load(n, (RESOURCES / "wav" / (n + ".wav")).string());

    score = std::make_unique<Score>((RESOURCES / "scores" / "marchin_in.txt").string());
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      shutdown();

    if (input->pressed("1"))
      timer->custom([&]() -> std::optional<double> {
        if (auto p = score->step()) {
          for (const auto &n : p->first)
            audio->play(n);
          debug_log(hades::join(" ", p->first));
          return p->second;
        }
        return std::nullopt;
      });
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
        .size = {400, 400},
        .monitor = 1,
        .flags = hades::WFlags::centered
    })
    .init_gl()
    .start();
}