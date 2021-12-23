#include "hades/application.hpp"

#include "hades/util/memusage.hpp"

namespace hades {

void Application::initialize() {}
void Application::update(double dt) {}
void Application::draw() {}

void Application::shutdown() {
  window->close_();
}

void Application::start_frame_() {
  window->fbo_->bind();

  gfx->switch_to_batch_set_("default");
  gfx->clear_batches_();

  gfx->clear(gl::ClearMask::depth);
}

void Application::end_frame_() {
  gfx->draw_batches_(window->projection());

  window->fbo_->unbind();

  gfx->clear(gl::ClearMask::color | gl::ClearMask::depth);
  window->fbo_->copy_to_default_framebuffer();

  gfx->clear(gl::ClearMask::depth);
  draw_overlay_();

  glfwSwapBuffers(window->glfw_window_);
}

void Application::draw_overlay_() { 
  gfx->switch_to_batch_set_("overlay");
  gfx->clear_batches_();

  glm::vec2 base_pos{1.0f, 1.0f};

  auto stat_str = fmt::format("{:.2f} FPS", frame_counter_.fps());
  if (window->vsync())
    stat_str += " (vsync)";
  stat_str += fmt::format(" | {:.2f} MB", hades::get_memusage_mb());
  draw_overlay_text_with_bg_(base_pos, stat_str);

  draw_debug_log_();

  gfx->draw_batches_(window->projection());
}

void Application::draw_debug_log_() {
  double dt = frame_counter_.dt();

  glm::vec2 base_pos{1.0f, window->h() - 1};
  for (auto it = overlay_.log.lines.begin(); it != overlay_.log.lines.end(); it++) {
    if (it->timeout > 0) {
      it->timeout -= dt;
      if (it->timeout <= 0.0)
        it->should_show = false;
      else if (it->timeout > 0.0 && it->timeout < 1.0)
        it->opacity = int(255.0 * it->timeout);
    }

    if (it->should_show) {
      auto bounds = overlay_.font->calc_text_bounds(0.0f, 0.0f, it->msg);
      base_pos.y -= bounds.h + 2;

      gfx->rect(
        base_pos.x, base_pos.y, bounds.w + 2, bounds.h + 2,
        hades::rgba(0, 0, 0, 217 * (it->opacity / 255.0))
      );

      overlay_.font->render(
        base_pos.x + 1, base_pos.y + 1,
        hades::rgba(255, 255, 255, it->opacity), 
        it->msg
      );
    }
  }

  while (!overlay_.log.lines.empty() && !overlay_.log.lines.back().should_show)
    overlay_.log.lines.pop_back();
}

void Application::draw_overlay_text_with_bg_(glm::vec2 &base_pos, const std::string &text) {
  auto bounds = overlay_.font->calc_text_bounds(base_pos.x, base_pos.y, text);
  gfx->rect(
    bounds.x, bounds.y, bounds.w + 2, bounds.h + 2,
    hades::rgba(0, 0, 0, 217)
  );
  
  overlay_.font->render(
    base_pos.x + 1, base_pos.y + 1,
    hades::rgba(0xffffffff), 
    text
  );

  base_pos.y += bounds.h + 2;
}

void Application::draw_overlay_skip_line_(glm::vec2 &base_pos) {
  base_pos.y += overlay_.font->char_h() + 2;
}

void Application::debug_log_(fmt::string_view format, fmt::format_args args) {
  auto msg = fmt::vformat(format, args);
  overlay_.log.lines.push_front(msg);
}

/******************
 * INITIALIZATION *
 ******************/

void Application::open_(const WCfg &cfg, glm::ivec2 glversion) {
  window = std::make_unique<Window>();
  window->open_(cfg, glversion);
  window->wm_info_.vsync = set(cfg.flags, WFlags::vsync);

  input = std::make_unique<InputMgr>(window->glfw_window_);
  timer = std::make_unique<TimerMgr>();
}

void Application::initgl_(glm::ivec2 glversion) {
  glfwMakeContextCurrent(window->glfw_window_);

  ctx_ = new GladGLContext();
  int version = gladLoadGLContext(ctx_, glfwGetProcAddress);
  if (version == 0) {
    spdlog::critical("Failed to initialize OpenGL context");
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  int glad_major = GLAD_VERSION_MAJOR(version);
  int glad_minor = GLAD_VERSION_MINOR(version);
  if (glad_major != glversion.x || glad_minor != glversion.y)
    spdlog::warn(
        "Requested OpenGL v{}.{}, got v{}.{}",
        glversion.x, glversion.y,
        glad_major, glad_minor
    );

  spdlog::debug("Initialized OpenGL context");
  spdlog::debug("=> Version: {}", ctx_->GetString(GL_VERSION));
  spdlog::debug("=> Vendor: {}", ctx_->GetString(GL_VENDOR));
  spdlog::debug("=> Renderer: {}", ctx_->GetString(GL_RENDERER));

  window->ctx_ = ctx_;

  gfx = std::make_unique<GfxMgr>(ctx_);

  glfwSwapInterval(window->vsync() ? 1 : 0);

  gfx->blend_func_(gl::BlendFunc::src_alpha, gl::BlendFunc::one_minus_src_alpha);
  gfx->enable_(gl::Capability::depth_test);

  ctx_->Enable(GL_TEXTURE_2D);

  window->create_fbo_();

  gfx->new_batch_set_("overlay", true);
  overlay_.font = gfx->load_cp437(
      (hades::RESOURCE_PATH / "font" / "1px_redux_7x9_no_bg.png").string(),
      7, 9,
      true
  );
  gfx->switch_to_batch_set_("default");
}

} // namespace hades