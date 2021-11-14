#include "hades/gl/context.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

namespace gl {

Context::Context(const std::string &tag, glm::ivec2 glversion) : tag(tag) {
    ctx_ = new GladGLContext();
    int version = gladLoadGLContext(ctx_, glfwGetProcAddress);
    if (version == 0) {
        spdlog::critical("({}) Failed to initialize OpenGL context", tag);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    int glad_major = GLAD_VERSION_MAJOR(version);
    int glad_minor = GLAD_VERSION_MINOR(version);
    if (glad_major != glversion.x || glad_minor != glversion.y)
        spdlog::warn("({}) Requested OpenGL v{}.{}, got v{}.{}", tag, glversion.x, glversion.y, glad_major, glad_minor);
    spdlog::debug(
        "({}) Initialized OpenGL context\n* Version: {}\n* Vendor: {}\n* Renderer: {}",
        tag,
        ctx_->GetString(GL_VERSION),
        ctx_->GetString(GL_VENDOR),
        ctx_->GetString(GL_RENDERER)
    );
}

Context::~Context() {
    delete ctx_;
}

Context::Context(Context &&other) noexcept {
    ctx_ = other.ctx_;
    other.ctx_ = nullptr;
}

Context &Context::operator=(Context &&other) noexcept {
    if (this != &other) {
        delete ctx_;
        ctx_ = other.ctx_;
        other.ctx_ = nullptr;
    }
    return *this;
}

void Context::enable(Capability cap) {
    ctx_->Enable(unwrap(cap));
}

void Context::disable(Capability cap) {
    ctx_->Disable(unwrap(cap));
}

void Context::depth_func(DepthFunc func) {
    ctx_->DepthFunc(unwrap(func));
}

void Context::blend_func(BlendFunc src, BlendFunc dst) {
    ctx_->BlendFunc(unwrap(src), unwrap(dst));
}

void Context::viewport(int x, int y, int w, int h) {
    ctx_->Viewport(x, y, w, h);
}

void Context::clip_control(ClipOrigin origin, ClipDepth depth) {
    ctx_->ClipControl(unwrap(origin), unwrap(depth));
}

void Context::clear_color(const hades::RGB &color) {
    ctx_->ClearColor(
        static_cast<float>(color.r) / 255.0f,
        static_cast<float>(color.g) / 255.0f,
        static_cast<float>(color.b) / 255.0f,
        static_cast<float>(color.a) / 255.0f
    );
}

void Context::flush() {
    ctx_->Flush();
}

void Context::clear(ClearMask mask) {
    ctx_->Clear(unwrap(mask));
}

ShaderBuilder Context::shader() {
    return {ctx_};
}

ShaderBuilder Context::shader(const std::string &tag) {
    return {ctx_, tag};
}

FramebufferBuilder Context::framebuffer(GLsizei width, GLsizei height) {
    return {ctx_, width, height};
}

std::unique_ptr<VertexArray> Context::vertex_array() {
    return std::make_unique<VertexArray>(ctx_);
}

} // namespace gl