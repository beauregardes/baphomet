#include "baphomet/app/application.hpp"

#include "baphomet/util/memusage.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace baphomet {

Application::Application() : Endpoint() {}

Application::~Application() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext(imgui_state_.ctx);

  imgui_state_.ctx = nullptr;
}

void Application::initialize() {}
void Application::update(Duration dt) {}
void Application::draw() {}

void Application::shutdown() {
  window->close_();
}

void Application::received_msg(const MsgCategory &category, const std::any &payload) {
  switch (category) {
    using enum MsgCategory;

    default:
      Endpoint::received_msg(category, payload);
  }
}

void Application::imgui_startframe_() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  imgui_state_.newframe_called = true;
}

void Application::imgui_endframe_() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  imgui_state_.newframe_called = false;
}

void Application::start_frame_() {
  window->fbo_->bind();

  gfx->switch_to_batch_set_("default");
  gfx->clear_batches_();

  gfx->clear(gl::ClearMask::depth);

  imgui_startframe_();
}

void Application::end_frame_() {
  gfx->draw_batches_(window->projection());

  imgui_endframe_();

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
#ifdef DEBUG
  stat_str += " (debug)";
#else
  stat_str += " (release)";
#endif
  stat_str += fmt::format(" | {:.2f} MB", get_memusage_mb());
  draw_overlay_text_with_bg_(base_pos, stat_str);

  draw_debug_log_();

  gfx->draw_batches_(window->projection());
}

void Application::draw_debug_log_() {
  Duration dt = frame_counter_.dt();

  glm::vec2 base_pos{1.0f, window->h() - 1};

  for (auto & line : overlay_.log.lines) {
    if (line.timeout > baphomet::sec(0)) {
      line.timeout -= dt;
      if (line.timeout <= baphomet::sec(0))
        line.should_show = false;
      else if (line.timeout < baphomet::sec(1))
        line.opacity = int(255.0 * line.timeout.count());
    }

    if (line.should_show) {
      auto bounds = overlay_.font->calc_text_bounds(0.0f, 0.0f, line.msg);
      base_pos.y -= bounds.h + 2;

      gfx->rect(
          base_pos.x, base_pos.y, bounds.w + 2, bounds.h + 2,
          baphomet::rgba(0, 0, 0, 217 * (line.opacity / 255.0))
      );

      overlay_.font->render(
          base_pos.x + 1, base_pos.y + 1,
          baphomet::rgba(255, 255, 255, line.opacity),
          line.msg
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
      baphomet::rgba(0, 0, 0, 217)
  );

  overlay_.font->render(
      base_pos.x + 1, base_pos.y + 1,
      baphomet::rgba(0xffffffff),
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

void Application::open_for_gl_(const WCfg &cfg, glm::ivec2 glversion) {
  initialize_endpoint(messenger_, MsgEndpoint::Application);

  window = std::make_unique<Window>(messenger_);
  window->open_for_gl_(cfg, glversion);
  window->wm_info_.vsync = set(cfg.flags, WFlags::vsync);

  input = std::make_unique<InputMgr>(window->glfw_window_, messenger_);

  audio = std::make_unique<AudioMgr>(messenger_);
  audio->open_device();
  audio->open_context();
  audio->make_current();

  timer = std::make_unique<TimerMgr>(messenger_);
  tween = std::make_unique<TweenMgr>(messenger_);
}

void Application::init_gl_(glm::ivec2 glversion) {
  glfwMakeContextCurrent(window->glfw_window_);

  int version = gladLoadGL(glfwGetProcAddress);
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
  spdlog::debug("=> Version: {}", glGetString(GL_VERSION));
  spdlog::debug("=> Vendor: {}", glGetString(GL_VENDOR));
  spdlog::debug("=> Renderer: {}", glGetString(GL_RENDERER));

  gfx = std::make_unique<GfxMgr>();

  glfwSwapInterval(window->vsync() ? 1 : 0);

  gfx->blend_func_(gl::BlendFunc::src_alpha, gl::BlendFunc::one_minus_src_alpha);
  gfx->enable_(gl::Capability::depth_test);

  glEnable(GL_TEXTURE_2D);

  window->create_fbo_(window->w(), window->h());

  gfx->new_batch_set_("overlay", true);
  overlay_.font = gfx->load_cp437(
      ResourceLoader::resolve_resource_path("fonts/1px_7x9.png"),
      7, 9,
      true
  );
  gfx->switch_to_batch_set_("default");
}

void Application::init_imgui_(glm::ivec2 glversion) {
  // Setup ImGui
  IMGUI_CHECKVERSION();
  imgui_state_.ctx = ImGui::CreateContext();
  imgui_state_.io = &ImGui::GetIO(); (void)imgui_state_.io;
  imgui_state_.io->IniFilename = nullptr;

  ImGui_ImplGlfw_InitForOpenGL(window->glfw_window_, true);
  std::string glsl_version;
  if      (glversion == glm::ivec2{2, 0}) glsl_version = "#version 110";
  else if (glversion == glm::ivec2{2, 1}) glsl_version = "#version 120";
  else if (glversion == glm::ivec2{3, 0}) glsl_version = "#version 130";
  else if (glversion == glm::ivec2{3, 1}) glsl_version = "#version 140";
  else if (glversion == glm::ivec2{3, 2}) glsl_version = "#version 150";
  else
    glsl_version = fmt::format("#version {}{}0", glversion.x, glversion.y);
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());

  spdlog::debug("Initialized ImGui");
}

} // namespace baphomet
