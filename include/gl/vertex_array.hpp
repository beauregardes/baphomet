#ifndef GL_VERTEX_ARRAY_HPP
#define GL_VERTEX_ARRAY_HPP

#include "glad/gl.h"

#include "buffer_base.hpp"
#include "hades/internal/bitmask_enum.hpp"

#include <vector>

namespace gl {

enum class AttrType {
    byte_t = GL_BYTE,
    ubyte_t = GL_UNSIGNED_BYTE,
    short_t = GL_SHORT,
    ushort_t = GL_UNSIGNED_SHORT,
    int_t = GL_INT,
    uint_t = GL_UNSIGNED_INT,
    hfloat_t = GL_HALF_FLOAT,
    float_t = GL_FLOAT,
    double_t = GL_DOUBLE,
    fixed_t = GL_FIXED,
    int_2_10_10_10_rev = GL_INT_2_10_10_10_REV,
    uint_2_10_10_10_rev = GL_UNSIGNED_INT_2_10_10_10_REV,
    uint_10f_11f_11f_rev = GL_UNSIGNED_INT_10F_11F_11F_REV
};

enum class DrawMode {
    points = GL_POINTS,
    line_strip = GL_LINE_STRIP,
    line_loop = GL_LINE_LOOP,
    lines = GL_LINES,
    line_strip_adjacency = GL_LINE_STRIP_ADJACENCY,
    lines_adjacency = GL_LINES_ADJACENCY,
    triangle_strip = GL_TRIANGLE_STRIP,
    triangle_fan = GL_TRIANGLE_FAN,
    triangles = GL_TRIANGLES,
    triangle_strip_adjacency = GL_TRIANGLE_STRIP_ADJACENCY,
    triangles_adjacency = GL_TRIANGLES_ADJACENCY,
    patches = GL_PATCHES
};

struct AttrDef {
    GLuint index;
    GLint size;
    AttrType type;
    bool normalized;
    GLsizei stride;
    GLsizei offset;
};

class VertexArray {
public:
    GLuint id {0};

    VertexArray(GladGLContext *ctx);

    ~VertexArray();

    // Copy constructors don't make sense for OpenGL objects
    VertexArray(const VertexArray &) = delete;
    VertexArray &operator=(const VertexArray &) = delete;

    VertexArray(VertexArray &&other) noexcept;
    VertexArray &operator=(VertexArray &&other) noexcept;

    void bind();
    void unbind();

    void attrib_pointer(const BufferBase *buffer, const std::vector<AttrDef> &definitions);
    void attrib_pointer(const BufferBase *buffer, const AttrDef &definition);

    void draw_arrays(DrawMode mode, GLint first, GLsizei count);

private:
    GladGLContext *ctx_{nullptr};

    void setup_enable_definition_(const AttrDef &definition);

    void gen_id_();
    void del_id_();
};

}  // namespace gl

#endif //GL_VERTEX_ARRAY_HPP
