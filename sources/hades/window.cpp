#include "hades/window.hpp"

#include "hades/util/platform.hpp"

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

void Window::window_set_auto_iconify(bool auto_iconify) {
    glfwSetWindowAttrib(glfw_window_, GLFW_AUTO_ICONIFY, auto_iconify ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::window_auto_iconify() {
    return glfwGetWindowAttrib(glfw_window_, GLFW_AUTO_ICONIFY) == GLFW_TRUE;
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

void Window::window_set_decorated(bool decorated) {
    glfwSetWindowAttrib(glfw_window_, GLFW_DECORATED, decorated ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::window_decorated() {
    return glfwGetWindowAttrib(glfw_window_, GLFW_DECORATED) == GLFW_TRUE;
}

void Window::window_set_vsync(bool vsync) {
    wm_info_.vsync = vsync;
    glfwSwapInterval(wm_info_.vsync ? 1 : 0);
}

bool Window::window_vsync() {
    return wm_info_.vsync;
}

void Window::window_set_size(int width, int height) {
    glfwSetWindowSize(glfw_window_, width, height);
}

glm::ivec2 Window::window_size() {
    int width, height;
    glfwGetWindowSize(glfw_window_, &width, &height);
    return {width, height};
}

void Window::window_set_size_limits(int min_width, int min_height, int max_width, int max_height) {
    glfwSetWindowSizeLimits(glfw_window_, min_width, min_height, max_width, max_height);
}

void Window::window_set_aspect_ratio(int numerator, int denominator) {
    glfwSetWindowAspectRatio(glfw_window_, numerator, denominator);
}

void Window::window_set_pos(int x, int y) {
    glfwSetWindowPos(glfw_window_, x, y);
}

glm::ivec2 Window::window_pos() {
    int x, y;
    glfwGetWindowPos(glfw_window_, &x, &y);
    return {x, y};
}

glm::mat4 Window::window_projection() {
    auto size = window_size();
    return glm::ortho(
        0.0f, static_cast<float>(size.x),
        static_cast<float>(size.y), 0.0f,
        0.0f, 1.0f
    );
}

void Window::start_frame_() {
    fbo_->bind();

    ctx->switch_to_batch_set("default");
    ctx->clear_batches();

    ctx->clear(gl::ClearMask::depth);
}

void Window::end_frame_() {
    ctx->draw_batches(window_projection());

    ctx->flush();
    ctx->clear(gl::ClearMask::depth);

    ctx->switch_to_batch_set("__overlay");
    ctx->clear_batches();

    overlay_.font->render(
        1, 2, 2.0f,
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

void Window::open_() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, open_cfg_.glversion.x);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, open_cfg_.glversion.y);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (set(open_cfg_.flags, WFlags::fullscreen) || set(open_cfg_.flags, WFlags::borderless)) {
#if (HADES_PLATFORM == WINDOWS)
        open_fullscreen_windows_();
#elif (HADES_PLATFORM == LINUX)
        open_fullscreen_linux();
#endif
    } else {
        open_windowed_();
    }

    events = std::make_unique<EventMgr>(glfw_window_);
    timers = std::make_unique<TimerMgr>();
}

GLFWmonitor *Window::get_monitor_() {
    int monitor_count = 0;
    auto monitors = glfwGetMonitors(&monitor_count);

    if (open_cfg_.monitor >= monitor_count) {
        spdlog::warn(
            "Monitor {} out of range (only {} monitors available); defaulting to primary ({})",
            open_cfg_.monitor,
            monitor_count,
            tag
        );
        return monitors[0];
    }
    return monitors[open_cfg_.monitor];
}

void Window::open_fullscreen_windows_() {
    GLFWmonitor *monitor = get_monitor_();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    if (set(open_cfg_.flags, WFlags::borderless)) {
        wm_info_.borderless = true;

        glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

        glfw_window_ = glfwCreateWindow(mode->width, mode->height, open_cfg_.title.c_str(), nullptr, nullptr);
    } else
        glfw_window_ = glfwCreateWindow(mode->width, mode->height, open_cfg_.title.c_str(), monitor, nullptr);

    if (!glfw_window_) {
        const char *description;
        int code = glfwGetError(&description);
        spdlog::critical("Failed to create GLFW window: ({})\n* ({}) {}", tag, code, description);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    } else
        spdlog::debug("Created GLFW window ({})", tag);

    if (set(open_cfg_.flags, WFlags::borderless)) {
        int base_x, base_y;
        glfwGetMonitorPos(monitor, &base_x, &base_y);
        glfwSetWindowPos(glfw_window_, base_x, base_y);
    }
}

void Window::open_fullscreen_linux_() {
    /* We are making the assumption that the user is running a version of X11
     * that treats *all* fullscreen windows as borderless fullscreen windows.
     * This seems to generally be true for a good majority of systems. This may
     * also just act exactly like a normal fullscreen, there's not really any
     * way to tell ahead of time. The trick with an undecorated window that
     * is the same size as the monitor only seems to work on Windows.
     */

    GLFWmonitor *monitor = get_monitor_();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    if (set(open_cfg_.flags, WFlags::borderless)) {
        wm_info_.borderless = true;

        glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    }

    glfw_window_ = glfwCreateWindow(mode->width + 1, mode->height + 1, open_cfg_.title.c_str(), monitor, nullptr);
    if (!glfw_window_) {
        const char *description;
        int code = glfwGetError(&description);
        spdlog::critical("Failed to create GLFW window: ({})\n* ({}) {}", tag, code, description);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    } else
        spdlog::debug("Created GLFW window ({})", tag);
}

void Window::open_windowed_() {
    GLFWmonitor *monitor = get_monitor_();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

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

    int base_x, base_y;
    glfwGetMonitorPos(monitor, &base_x, &base_y);
    if (set(open_cfg_.flags, WFlags::centered))
        glfwSetWindowPos(
            glfw_window_,
            base_x + (mode->width - open_cfg_.size.x) / 2,
            base_y + (mode->height - open_cfg_.size.y) / 2
         );
    else
        glfwSetWindowPos(
            glfw_window_,
            base_x + open_cfg_.position.x,
            base_y + open_cfg_.position.y
        );

    if (!set(open_cfg_.flags, WFlags::hidden))
        glfwShowWindow(glfw_window_);
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

    auto size = window_size();
    fbo_ = gl::FramebufferBuilder(ctx_, size.x, size.y)
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