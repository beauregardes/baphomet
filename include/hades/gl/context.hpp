#ifndef HADES_GL_CONTEXT_H
#define HADES_GL_CONTEXT_H

#include "glad/gl.h"
#include "glm/glm.hpp"

#include "hades/gl/context_enums.hpp"
#include "hades/gl/framebuffer.hpp"
#include "hades/gl/shader.hpp"
#include "hades/gl/static_buffer.hpp"
#include "hades/gl/vec_buffer.hpp"
#include "hades/gl/vertex_array.hpp"
#include "hades/color.hpp"

#include <string>

namespace gl {

class Context {
public:
    std::string tag;

    Context(const std::string &tag, glm::ivec2 glversion);
    ~Context();

    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;

    Context(Context &&other) noexcept;
    Context &operator=(Context &&other) noexcept;

    void enable(Capability cap);
    void disable(Capability cap);

    void depth_func(DepthFunc func);

    void blend_func(BlendFunc src, BlendFunc dst);

    void viewport(int x, int y, int w, int h);

    void clip_control(ClipOrigin origin, ClipDepth depth);

    void clear_color(const hades::RGB &color);
    void clear(ClearMask mask);

    void flush();

    ShaderBuilder shader();
    ShaderBuilder shader(const std::string &tag);

    FramebufferBuilder framebuffer(GLsizei width, GLsizei height);

    template<typename T>
    std::unique_ptr<StaticBuffer<T>> static_buffer(GLsizeiptr size, BufTarget target, BufUsage usage) {
        return std::make_unique<StaticBuffer<T>>(ctx_, size, target, usage);
    }

    template<typename T>
    std::unique_ptr<StaticBuffer<T>> static_buffer(const std::vector<T> &data, BufTarget target, BufUsage usage) {
        return std::make_unique<StaticBuffer<T>>(ctx_, data, target, usage);
    }

    template<typename T>
    std::unique_ptr<VecBuffer<T>> vec_buffer(BufTarget target, BufUsage usage, std::size_t initial_size, bool front_to_back = true) {
        return std::make_unique<VecBuffer<T>>(ctx_, target, usage, initial_size, front_to_back);
    }

    std::unique_ptr<VertexArray> vertex_array();

private:
    GladGLContext *ctx_{nullptr};
};

} // namespace hades::gl

#endif //HADES_GL_CONTEXT_H
