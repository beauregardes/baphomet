#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H

#include "glad/gl.h"
#include "glm/glm.hpp"

#include "gl/context_enums.hpp"
#include "gl/framebuffer.hpp"
#include "gl/shader.hpp"
#include "gl/static_buffer.hpp"
#include "gl/vec_buffer.hpp"
#include "gl/vertex_array.hpp"
#include "gl/batching/line_batch.hpp"
#include "gl/batching/oval_batch.hpp"
#include "gl/batching/pixel_batch.hpp"
#include "gl/batching/tri_batch.hpp"
#include "gl/batching/rect_batch.hpp"

#include "hades/color.hpp"
#include "hades/texture.hpp"
#include "hades/resource_loader.hpp"

#include <memory>
#include <string>

namespace gl {

class Context {
public:
    std::string tag;

    Context(GladGLContext *ctx, const std::string &tag);

    ~Context() = default;

    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;

    Context(Context &&other) noexcept;
    Context &operator=(Context &&other) noexcept;

    bool auto_clear_batches();
    void set_auto_clear_batches(bool s);

    void enable(Capability cap);
    void disable(Capability cap);

    void depth_func(DepthFunc func);

    void blend_func(BlendFunc src, BlendFunc dst);

    void viewport(int x, int y, int w, int h);

    void clip_control(ClipOrigin origin, ClipDepth depth);

    void clear_color(const hades::RGB &color);
    void clear(ClearMask mask);

    void flush();

    /***********
     * BATCHING
     */

    void clear_batches();
    void draw_batches(glm::mat4 projection);

    /*************
     * PRIMITIVES
     */

    void pixel(float x, float y, const hades::RGB &color);

    void line(float x0, float y0, float x1, float y1, const hades::RGB &color, float cx, float cy, float angle);
    void line(float x0, float y0, float x1, float y1, const hades::RGB &color, float angle);
    void line(float x0, float y0, float x1, float y1, const hades::RGB &color);

    void tri(float x0, float y0, float x1, float y1, float x2, float y2, const hades::RGB &color, float cx, float cy, float angle);
    void tri(float x0, float y0, float x1, float y1, float x2, float y2, const hades::RGB &color, float angle);
    void tri(float x0, float y0, float x1, float y1, float x2, float y2, const hades::RGB &color);

    void tri(float x, float y, float radius, const hades::RGB &color, float angle);
    void tri(float x, float y, float radius, const hades::RGB &color);

    void rect(float x, float y, float w, float h, const hades::RGB &color, float cx, float cy, float angle);
    void rect(float x, float y, float w, float h, const hades::RGB &color, float angle);
    void rect(float x, float y, float w, float h, const hades::RGB &color);

    void oval(float x, float y, float x_radius, float y_radius, const hades::RGB &color, float cx, float cy, float angle);
    void oval(float x, float y, float x_radius, float y_radius, const hades::RGB &color, float angle);
    void oval(float x, float y, float x_radius, float y_radius, const hades::RGB &color);

    void circle(float x, float y, float radius, const hades::RGB &color, float cx, float cy, float angle);
    void circle(float x, float y, float radius, const hades::RGB &color, float angle);
    void circle(float x, float y, float radius, const hades::RGB &color);

    /***********
     * TEXTURES
     */

    std::unique_ptr<hades::Texture> load_texture(const std::string &path, bool retro = false);

private:
    GladGLContext *ctx_{nullptr};

    std::unique_ptr<hades::ResourceLoader> resource_loader{nullptr};

    float z_level_{1.0f};
    std::unique_ptr<PixelBatch> pixels_{nullptr};
    std::unique_ptr<LineBatch> lines_{nullptr};
    std::unique_ptr<TriBatch> tris_{nullptr};
    std::unique_ptr<RectBatch> rects_{nullptr};
    std::unique_ptr<OvalBatch> ovals_{nullptr};
    std::unordered_map<std::string, std::unique_ptr<TextureBatch>> textures_{};

    bool auto_clear_batches_{true};
};

} // namespace hades::gl

#endif //GL_CONTEXT_H
