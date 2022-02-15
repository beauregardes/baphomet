#include "baphomet/app/runner.hpp"

#include "baphomet/app/internal/messenger.hpp"
#include "baphomet/util/platform.hpp"

#include "spdlog/spdlog.h"

namespace baphomet {

Runner::Runner(spdlog::level::level_enum log_level) {
  spdlog::set_level(log_level);

  if (!glfwInit()) {
    const char *description;
    int code = glfwGetError(&description);

    spdlog::critical(
        "Failed to initialize GLFW:\n* ({}) {}",
        code, description
    );
    std::exit(EXIT_FAILURE);
  } else
    spdlog::debug("Initialized GLFW");

  msgr_ = std::make_shared<Messenger>();

  msgr_->register_endpoint("RUNNER", [&](const MsgCat &category, const std::any &payload) {
    received_message_(category, payload);
  });
}

Runner::~Runner() {
  glfwTerminate();
}

Runner &Runner::init_gl(glm::ivec2 glversion) {
  open_params_.glversion = glversion;
  return *this;
}

void Runner::start() {
  application_->open_for_gl_(open_params_.cfg, open_params_.glversion);

  application_->init_gl_(open_params_.glversion);
  application_->init_imgui_(open_params_.glversion);

  application_->initialize();

  do {
    application_->frame_counter_.update();
    Duration dt = application_->frame_counter_.dt();

    application_->update(dt);

    application_->start_frame_();
    application_->draw();
    application_->end_frame_();

    msgr_->send_message<MsgCat::Update>("INPUT-MGR", dt);
    msgr_->send_message<MsgCat::Update>("TIMER-MGR", dt);
    msgr_->send_message<MsgCat::Update>("TWEEN-MGR", dt);
    msgr_->send_message<MsgCat::Update>("AUDIO-MGR", dt);

    glfwPollEvents();
  } while (!glfwWindowShouldClose(glfw_window_));
}

void Runner::received_message_(const MsgCat &category, const std::any &payload) {
  switch (category) {
    using enum MsgCat;

    case RegisterGlfwCallbacks: {
      auto p = Messenger::extract_payload<RegisterGlfwCallbacks>(payload);
      glfwSetWindowUserPointer(p.window, this);
      glfwSetKeyCallback(p.window, glfw_key_callback_);
      glfwSetCursorPosCallback(p.window, glfw_cursor_position_callback_);
      glfwSetCursorEnterCallback(p.window, glfw_cursor_enter_callback_);
      glfwSetMouseButtonCallback(p.window, glfw_mouse_button_callback_);
      glfwSetScrollCallback(p.window, glfw_scroll_callback_);
      glfwSetWindowSizeCallback(p.window, glfw_window_size_callback_);
      glfwSetWindowPosCallback(p.window, glfw_window_pos_callback_);
      glfwSetWindowFocusCallback(p.window, glfw_window_focus_callback_);

      // Save this for later--we'll need it to
      // check if we should quit the application
      glfw_window_ = p.window;
    }
      break;

    default:
      spdlog::error("RUNNER: Unhandled message category: '{}'", category);
  }
}

void Runner::glfw_key_callback_(GLFWwindow *window, int key, int scancode, int action, int mods) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->msgr_->send_message<MsgCat::KeyEvent>("INPUT-MGR", key, scancode, action, mods);
}

void Runner::glfw_cursor_position_callback_(GLFWwindow *window, double xpos, double ypos) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->msgr_->send_message<MsgCat::CursorPositionEvent>("INPUT-MGR", xpos, ypos);
}

void Runner::glfw_cursor_enter_callback_(GLFWwindow *window, int entered) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->msgr_->send_message<MsgCat::CursorEnterEvent>("INPUT-MGR", entered);
}

void Runner::glfw_mouse_button_callback_(GLFWwindow *window, int button, int action, int mods) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->msgr_->send_message<MsgCat::MouseButtonEvent>("INPUT-MGR", button, action, mods);
}

void Runner::glfw_scroll_callback_(GLFWwindow *window, double xoffset, double yoffset) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->msgr_->send_message<MsgCat::ScrollEvent>("INPUT-MGR", xoffset, yoffset);
}

void Runner::glfw_window_size_callback_(GLFWwindow *window, int width, int height) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->msgr_->send_message<MsgCat::WindowSize>("WINDOW", width, height);
}

void Runner::glfw_window_pos_callback_(GLFWwindow *window, int xpos, int ypos) { /* noop */ }

void Runner::glfw_window_focus_callback_(GLFWwindow *window, int focused) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->msgr_->send_message<MsgCat::WindowFocus>("WINDOW", focused);
}

} // namespace baphomet
