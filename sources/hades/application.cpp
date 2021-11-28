#include "hades/application.hpp"

namespace hades {

void Application::initialize() {}
void Application::update(double dt) {}
void Application::draw() {}

void Application::shutdown() {
  window->close_();
}

void Application::start_frame_() {
  fbo_->bind();

  gfx->switch_to_batch_set_("default");
  gfx->clear_batches_();

  gfx->clear(gl::ClearMask::depth);
}

void Application::end_frame_() {
  gfx->draw_batches_(window->projection());

  fbo_->unbind();

  gfx->clear(gl::ClearMask::color | gl::ClearMask::depth);
  fbo_->copy_to_default_framebuffer();

  gfx->clear(gl::ClearMask::depth);
  draw_overlay_();

  glfwSwapBuffers(window->glfw_window_);
}

void Application::draw_overlay_() {
  gfx->switch_to_batch_set_("__overlay");
  gfx->clear_batches_();

  auto fps_str = fmt::format("{:.2f} FPS", overlay_.frame_counter.fps());
  if (window->vsync())
    fps_str += " (vsync)";
  overlay_.font->render(1, 2, fps_str);

  auto batch_str = fmt::format(
    "pixels: {}\nlines: {}\ntris: {}\nrect: {}\novals: {}\ntexture: {}\n",
    gfx->pixel_count_(),
    gfx->line_count_() / 2,
    gfx->tri_count_() / 3,
    gfx->rect_count_() / 3,
    gfx->oval_count_() / 3,
    gfx->texture_count_() / 3
  );
  overlay_.font->render(1, 2 + overlay_.font->char_h() * 2, batch_str);

  gfx->draw_batches_(window->projection());
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

  gfx = std::make_unique<GfxMgr>(ctx_);

  glfwSwapInterval(window->vsync() ? 1 : 0);

  gfx->blend_func_(gl::BlendFunc::src_alpha, gl::BlendFunc::one_minus_src_alpha);

  gfx->enable_(gl::Capability::depth_test);

  fbo_ = gl::FramebufferBuilder(ctx_, window->w(), window->h())
    .renderbuffer(gl::RBufFormat::rgba8)
    .renderbuffer(gl::RBufFormat::d32f)
    .check_complete();

  gfx->new_batch_set_("__overlay", true);
  overlay_.font = gfx->load_cp437(
    (hades::RESOURCE_PATH / "font" / "1px_6x8_no_bg.png").string(),
    6, 8,
    true
  );
  gfx->switch_to_batch_set_("default");
}

} // namespace hades