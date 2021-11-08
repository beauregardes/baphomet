#ifndef HADES_GL_CONTEXT_H
#define HADES_GL_CONTEXT_H

#include "glad/gl.h"
#include "glm/glm.hpp"

#include "hades/internal/bitmask_enum.hpp"
#include "hades/color.hpp"

#include <string>

namespace gl {

enum class ClearBit {
    color = GL_COLOR_BUFFER_BIT,
    depth = GL_DEPTH_BUFFER_BIT,
    stencil = GL_STENCIL_BUFFER_BIT
};

class Context {
public:
    std::string tag;

    Context(const std::string &tag, glm::ivec2 glversion);
    ~Context();

    void clear_color(const hades::RGB &color);

    void clear(ClearBit mask);

private:
    GladGLContext *ctx_{nullptr};
};

} // namespace hades::gl

ENABLE_BITMASK_OPERATORS(gl::ClearBit)

#endif //HADES_GL_CONTEXT_H
