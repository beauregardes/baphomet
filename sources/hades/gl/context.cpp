#include "hades/gl/context.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

namespace gl {

Context::Context(const std::string &tag, glm::ivec2 glversion) {
    this->tag = tag;

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

void Context::clear_color(const hades::RGB &color) {
    ctx_->ClearColor(
        static_cast<float>(color.r) / 255.0f,
        static_cast<float>(color.g) / 255.0f,
        static_cast<float>(color.b) / 255.0f,
        static_cast<float>(color.a) / 255.0f
    );
}

void Context::clear(ClearBit mask) {
    ctx_->Clear(unwrap(mask));
}

} // namespace gl