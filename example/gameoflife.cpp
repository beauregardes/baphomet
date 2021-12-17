#include "hades/hades.hpp"

class GameOfLife : public hades::Application {
public:
  const int CELL_SIZE{5};
  const int CELL_ROWS{150};
  const int CELL_COLS{150};
  const int WIN_PADW{CELL_SIZE * 2};
  const int WIN_PADH{CELL_SIZE * 2};

  const hades::RGB BG_COLOR{hades::rgb(0x040404)};
  const hades::RGB GRID_COLOR{hades::rgb(0x101010)};
  const hades::RGB CELL_COLOR{hades::rgb(0xeeeeee)};

  struct CellState {
    bool curr{false};
    bool prev{false};
  };
  std::vector<std::vector<CellState>> cells;

  std::vector<int> stay{1, 2, 3, 4, 5};
  std::vector<int> born{3};

  void initialize() override {
    window->set_size(
        WIN_PADW + CELL_COLS * CELL_SIZE + CELL_COLS + 1 + WIN_PADW,
        WIN_PADH + CELL_ROWS * CELL_SIZE + CELL_ROWS + 1 + WIN_PADH
    );
    window->center(1);
    window->set_visible(true);

    cells = std::vector<std::vector<CellState>>(
        CELL_ROWS,
        std::vector<CellState>(
            CELL_COLS
        )
    );

    timer->every("Simulate", 1.0/60.0, [&]{ step_simulate(); });
    timer->pause("Simulate");
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      shutdown();

    if (input->pressed("r"))
      reset_cells();

    if (input->pressed("s"))
      timer->toggle("Simulate");

    if (input->pressed("t"))
      step_simulate();

    if (input->down("mb_left") && input->mouse.moved)
      set_cell(true);

    if (input->down("mb_right") && input->mouse.moved)
      set_cell(false);
  }

  void draw() override {
    gfx->clear_color(BG_COLOR);
    gfx->clear();

    draw_grid();
    draw_cells();

    if (is_in_grid(input->mouse.x, input->mouse.y)) {
      auto cell_pos = mouse_pos_to_cell_pos(input->mouse.x, input->mouse.y);
      draw_cell(cell_pos.y, cell_pos.x, hades::rgba(CELL_COLOR.r, CELL_COLOR.g, CELL_COLOR.b, 128));
    }
  }

  void reset_cells() {
    for (int r = 0; r < CELL_ROWS; ++r)
      for (int c = 0; c < CELL_COLS; ++c)
        cells[r][c].curr = false;
  }

  bool is_in_grid(int x, int y) {
    return x >= WIN_PADW &&
           y >= WIN_PADH &&
           x < window->w() - WIN_PADW &&
           y < window->h() - WIN_PADH;
  }

  glm::ivec2 mouse_pos_to_cell_pos(int mouse_x, int mouse_y) {
    mouse_x -= WIN_PADW;
    mouse_y -= WIN_PADH;
    auto cell_col = static_cast<int>(mouse_x / (CELL_SIZE + 1));
    auto cell_row = static_cast<int>(mouse_y / (CELL_SIZE + 1));
    return {cell_col, cell_row};
  }

  void set_cell(bool state) {
    if (is_in_grid(input->mouse.px, input->mouse.py) && is_in_grid(input->mouse.x, input->mouse.y)) {
      auto start_cell = mouse_pos_to_cell_pos(input->mouse.px, input->mouse.py);
      auto end_cell = mouse_pos_to_cell_pos(input->mouse.x, input->mouse.y);
      hades::helpers::bresenham(
          start_cell.x, start_cell.y,
          end_cell.x, end_cell.y,
          [&](int x, int y) {
            cells[y][x].curr = state;
          }
      );
    }
  }

  int count_neighbors(int r, int c) {
    int sr = (r == 0) ? r : r - 1;
    int er = (r == CELL_ROWS - 1) ? r : r + 1;

    int sc = (c == 0) ? c : c - 1;
    int ec = (c == CELL_COLS - 1) ? c : c + 1;

    int n = 0;
    for (int cr = sr; cr <= er; ++cr)
      for (int cc = sc; cc <= ec; ++cc)
        if (!(cr == r && cc == c) && cells[cr][cc].prev)
          n++;
    return n;
  }

  void step_simulate() {
    for (int r = 0; r < CELL_ROWS; ++r)
      for (int c = 0; c < CELL_COLS; ++c)
        cells[r][c].prev = cells[r][c].curr;

    for (int r = 0; r < CELL_ROWS; ++r)
      for (int c = 0; c < CELL_COLS; ++c) {
        int n = count_neighbors(r, c);
        if (cells[r][c].prev) {
          if (std::find(stay.begin(), stay.end(), n) == stay.end())
            cells[r][c].curr = false;
        } else {
          if (std::find(born.begin(), born.end(), n) != born.end())
            cells[r][c].curr = true;
        }
      }
  }

  void draw_grid() {
    for (int r = 0; r <= CELL_ROWS; ++r)
      gfx->line(
          WIN_PADW,
          WIN_PADH + r * CELL_SIZE + r,
          window->w() - WIN_PADW,
          WIN_PADH + r * CELL_SIZE + r,
          GRID_COLOR
      );
    for (int c = 0; c <= CELL_COLS; ++c)
      gfx->line(
          WIN_PADW + c * CELL_SIZE + c,
          WIN_PADH,
          WIN_PADW + c * CELL_SIZE + c,
          window->h() - WIN_PADH,
          GRID_COLOR
      );
  }

  void draw_cell(int r, int c, const hades::RGB &color) {
    gfx->rect(
        WIN_PADH + c * CELL_SIZE + c + 1,
        WIN_PADW + r * CELL_SIZE + r + 1,
        CELL_SIZE,
        CELL_SIZE,
        color
    );
  }

  void draw_cells() {
    for (int r = 0; r < CELL_ROWS; ++r)
      for (int c = 0; c < CELL_COLS; ++c)
        if (cells[r][c].curr)
          draw_cell(r, c, CELL_COLOR);
  }
};

int main(int, char *[]) {
  hades::Runner()
      .open<GameOfLife>({
          .title = "Game Of Life",
          .flags = hades::WFlags::hidden
      })
      .initgl()
      .start();
}
