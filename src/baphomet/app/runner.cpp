#include "baphomet/app/runner.hpp"

#include "spdlog/spdlog.h"

namespace baphomet {

Runner::Runner(spdlog::level::level_enum log_level) : Endpoint() {
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

  messenger_ = std::make_shared<Messenger>();
  initialize_endpoint(messenger_, MsgEndpoint::Runner);
}

Runner::~Runner() {
  glfwTerminate();
}

Runner &Runner::init_gl(glm::ivec2 glversion) {
  open_params_.glversion = glversion;
  return *this;
}

void Runner::start() {
  application_->open_for_gl_(messenger_, open_params_.cfg, open_params_.glversion);

  application_->init_gl_(open_params_.glversion);
  application_->init_imgui_(open_params_.glversion);

  application_->initialize();

  do {
    application_->frame_counter_.update();
    Duration dt = application_->frame_counter_.dt();

    // User defined update, doesn't update systems
    application_->update(dt);

    application_->start_frame_();
    application_->draw();
    application_->end_frame_();

    // Non-user defined update, updates systems (input, audio, etc)
    application_->update_nonuser_(dt);

    application_->poll_events_();
  } while (!application_->should_close_());
}

void Runner::received_msg(const MsgCategory &category, const std::any &payload) {
  switch (category) {
    using enum MsgCategory;

    case RegisterGlfwCallbacks: {
      // The callbacks all exist in a single place because we can only set one
      // user pointer for the GLFW window. The only common place where
      // everything is involved is the runner, so that's where they get to live

      auto p = extract_msg_payload<RegisterGlfwCallbacks>(payload);

      glfwSetWindowUserPointer(p.window, this);
      glfwSetKeyCallback(p.window, glfw_key_callback_);
      glfwSetCursorPosCallback(p.window, glfw_cursor_position_callback_);
      glfwSetCursorEnterCallback(p.window, glfw_cursor_enter_callback_);
      glfwSetMouseButtonCallback(p.window, glfw_mouse_button_callback_);
      glfwSetScrollCallback(p.window, glfw_scroll_callback_);
      glfwSetWindowSizeCallback(p.window, glfw_window_size_callback_);
      glfwSetWindowPosCallback(p.window, glfw_window_pos_callback_);
      glfwSetWindowFocusCallback(p.window, glfw_window_focus_callback_);
    }
      break;

    default:
      Endpoint::received_msg(category, payload);
  }
}

void Runner::glfw_key_callback_(GLFWwindow *window, int key, int scancode, int action, int mods) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->send_msg<MsgCategory::KeyEvent>(MsgEndpoint::Input, key, scancode, action, mods);
}

void Runner::glfw_cursor_position_callback_(GLFWwindow *window, double xpos, double ypos) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->send_msg<MsgCategory::CursorPositionEvent>(MsgEndpoint::Input, xpos, ypos);
}

void Runner::glfw_cursor_enter_callback_(GLFWwindow *window, int entered) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->send_msg<MsgCategory::CursorEnterEvent>(MsgEndpoint::Input, entered);
}

void Runner::glfw_mouse_button_callback_(GLFWwindow *window, int button, int action, int mods) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->send_msg<MsgCategory::MouseButtonEvent>(MsgEndpoint::Input, button, action, mods);
}

void Runner::glfw_scroll_callback_(GLFWwindow *window, double xoffset, double yoffset) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->send_msg<MsgCategory::ScrollEvent>(MsgEndpoint::Input, xoffset, yoffset);
}

void Runner::glfw_window_size_callback_(GLFWwindow *window, int width, int height) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->send_msg<MsgCategory::WindowSize>(MsgEndpoint::Window, width, height);
}

void Runner::glfw_window_pos_callback_(GLFWwindow *window, int xpos, int ypos) { /* noop */ }

void Runner::glfw_window_focus_callback_(GLFWwindow *window, int focused) {
  auto runner = reinterpret_cast<Runner *>(glfwGetWindowUserPointer(window));
  runner->send_msg<MsgCategory::WindowFocus>(MsgEndpoint::Window, focused);
}

} // namespace baphomet
