#include <hades/window.hpp>
#include "hades/window.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "spdlog/spdlog.h"

namespace hades {

Window::~Window() {
    mgr = nullptr;
}

void Window::initialize() {}
void Window::update(double dt) {}
void Window::draw() {}

void Window::window_close() {
    glfwSetWindowShouldClose(glfw_window_, GLFW_TRUE);
}

int Window::window_x() const {
    return wm_info_.pos.x;
}

int Window::window_y() const {
    return wm_info_.pos.y;
}

int Window::window_width() const {
    return wm_info_.size.x;
}

int Window::window_height() const {
    return wm_info_.size.y;
}

void Window::window_set_floating(bool floating) {
    glfwSetWindowAttrib(glfw_window_, GLFW_FLOATING, floating ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::window_floating() {
    return glfwGetWindowAttrib(glfw_window_, GLFW_FLOATING) == GLFW_TRUE;
}

void Window::window_set_resizable(bool resizable) {
    glfwSetWindowAttrib(glfw_window_, GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::window_resizable() {
    return glfwGetWindowAttrib(glfw_window_, GLFW_RESIZABLE) == GLFW_TRUE;
}

void Window::window_set_visible(bool visible) {
    glfwSetWindowAttrib(glfw_window_, GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::window_visible() {
    return glfwGetWindowAttrib(glfw_window_, GLFW_VISIBLE) == GLFW_TRUE;
}

void Window::window_set_vsync(bool vsync) {
    wm_info_.vsync = vsync;
    glfwSwapInterval(wm_info_.vsync ? 1 : 0);
}

bool Window::window_vsync() {
    return wm_info_.vsync;
}

glm::mat4 Window::window_projection() const {
    return glm::ortho(
        0.0f, static_cast<float>(wm_info_.size.x),
        static_cast<float>(wm_info_.size.y), 0.0f,
        0.0f, 1.0f
    );
}

void Window::start_frame_() {
    fbo_->bind();

    ctx->switch_to_batch_set("default");
    ctx->clear_batches();
}

void Window::end_frame_() {
    ctx->draw_batches(window_projection());

    ctx->flush();

    ctx->switch_to_batch_set("__overlay");
    ctx->clear_batches();

    overlay_.font->render(
        1, 2,
        "{:.2f} fps{}",
        overlay_.frame_counter.fps(),
        wm_info_.vsync ? " (vsync)" : ""
    );

    ctx->draw_batches(window_projection());

    fbo_->unbind();
    fbo_->copy_to_default_framebuffer();
    glfwSwapBuffers(glfw_window_);
}

void Window::make_current_() {
    glfwMakeContextCurrent(glfw_window_);
}

#define _NET_WM_STATE_REMOVE        0    // remove/unset property
#define _NET_WM_STATE_ADD           1    // add/set property
#define _NET_WM_STATE_TOGGLE        2    // toggle property

void Window::open_() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, open_cfg_.glversion.x);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, open_cfg_.glversion.y);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int monitor_count = 0;
    auto monitors = glfwGetMonitors(&monitor_count);

    GLFWmonitor *monitor;
    if (open_cfg_.monitor >= monitor_count) {
        spdlog::warn(
            "Monitor {} out of range (only {} monitors available); defaulting to primary ({})",
            open_cfg_.monitor,
            monitor_count,
            tag
        );
        monitor = monitors[0];
    } else
        monitor = monitors[open_cfg_.monitor];
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    if (set(open_cfg_.flags, WFlags::fullscreen)) {
        glfw_window_ = glfwCreateWindow(mode->width, mode->height, open_cfg_.title.c_str(), monitor, nullptr);
        if (!glfw_window_) {
            const char *description;
            int code = glfwGetError(&description);
            spdlog::critical("Failed to create GLFW window: ({})\n* ({}) {}", tag, code, description);
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        } else
            spdlog::debug("Created GLFW window ({})", tag);
        wm_info_.size = {mode->width, mode->height};

    } else if (set(open_cfg_.flags, WFlags::borderless)) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

        glfw_window_ = glfwCreateWindow(mode->width, mode->height, open_cfg_.title.c_str(), nullptr, nullptr);
        if (!glfw_window_) {
            const char *description;
            int code = glfwGetError(&description);
            spdlog::critical("Failed to create GLFW window: ({})\n* ({}) {}", tag, code, description);
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        } else
            spdlog::debug("Created GLFW window ({})", tag);
        wm_info_.size = {mode->width, mode->height};
        wm_info_.borderless = true;

        int base_x, base_y;
        glfwGetMonitorPos(monitor, &base_x, &base_y);
        wm_info_.pos = {base_x, base_y};
        glfwSetWindowPos(glfw_window_, wm_info_.pos.x, wm_info_.pos.y);

    } else {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, set(open_cfg_.flags, WFlags::resizable) ? GLFW_TRUE : GLFW_FALSE);

        glfw_window_ = glfwCreateWindow(open_cfg_.size.x, open_cfg_.size.y, open_cfg_.title.c_str(), nullptr, nullptr);
        if (!glfw_window_) {
            const char *description;
            int code = glfwGetError(&description);
            spdlog::critical("Failed to create GLFW window: ({})\n* ({}) {}", tag, code, description);
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        } else
            spdlog::debug("Created GLFW window ({})", tag);
        wm_info_.size = open_cfg_.size;

        int base_x, base_y;
        glfwGetMonitorPos(monitor, &base_x, &base_y);
        if (set(open_cfg_.flags, WFlags::centered))
            wm_info_.pos = {base_x + (mode->width - wm_info_.size.x) / 2,
                            base_y + (mode->height - wm_info_.size.y) / 2};
        else
            wm_info_.pos = {base_x + open_cfg_.position.x,
                            base_y + open_cfg_.position.y};
        glfwSetWindowPos(glfw_window_, wm_info_.pos.x, wm_info_.pos.y);

        if (!set(open_cfg_.flags, WFlags::hidden))
            glfwShowWindow(glfw_window_);
    }

    events = std::make_unique<EventMgr>(glfw_window_);
    timers = std::make_unique<TimerMgr>();
}

void Window::initialize_gl_() {
    make_current_();

    ctx_ = new GladGLContext();
    int version = gladLoadGLContext(ctx_, glfwGetProcAddress);
    if (version == 0) {
        spdlog::critical("Failed to initialize OpenGL context ({})", tag);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    int glad_major = GLAD_VERSION_MAJOR(version);
    int glad_minor = GLAD_VERSION_MINOR(version);
    if (glad_major != open_cfg_.glversion.x || glad_minor != open_cfg_.glversion.y)
        spdlog::warn(
            "Requested OpenGL v{}.{}, got v{}.{} ({})",
            open_cfg_.glversion.x, open_cfg_.glversion.y,
            glad_major, glad_minor,
            tag
        );

    spdlog::debug("Initialized OpenGL context ({})", tag);
    spdlog::debug("=> Version: {}", ctx_->GetString(GL_VERSION));
    spdlog::debug("=> Vendor: {}", ctx_->GetString(GL_VENDOR));
    spdlog::debug("=> Renderer: {}", ctx_->GetString(GL_RENDERER));

    ctx = std::make_unique<Context>(ctx_, tag);

    wm_info_.vsync = set(open_cfg_.flags, WFlags::vsync);
    glfwSwapInterval(wm_info_.vsync ? 1 : 0);

//    ctx->enable(gl::Capability::blend);
    ctx->blend_func(gl::BlendFunc::src_alpha, gl::BlendFunc::one_minus_src_alpha);

    ctx->enable(gl::Capability::depth_test);

    fbo_ = gl::FramebufferBuilder(ctx_, window_width(), window_height())
        .renderbuffer(gl::RBufFormat::rgba8)
        .renderbuffer(gl::RBufFormat::d32f)
        .check_complete();

    ctx->new_batch_set("__overlay");
    ctx->switch_to_batch_set("__overlay");
    overlay_.font = ctx->load_cp437(
        (hades::RESOURCE_PATH / "font" / "1px_6x8_no_bg.png").string(),
        6, 8,
        true
    );
    ctx->switch_to_batch_set("default");
}

} // namespace hades