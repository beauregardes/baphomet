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

#include "hades/batch_set.hpp"
#include "hades/color.hpp"
#include "hades/texture.hpp"
#include "hades/resource_loader.hpp"
#include "hades/font/cp437.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace hades {

class Context {
public:
    std::string tag;

    Context(GladGLContext *ctx, const std::string &tag);

    ~Context() = default;

    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;

    Context(Context &&other) noexcept;
    Context &operator=(Context &&other) noexcept;

    void enable(gl::Capability cap);
    void disable(gl::Capability cap);

    void depth_func(gl::DepthFunc func);
    void depth_mask(bool flag);

    void blend_func(gl::BlendFunc src, gl::BlendFunc dst);

    void viewport(int x, int y, int w, int h);

    void clip_control(gl::ClipOrigin origin, gl::ClipDepth depth);

    void clear_color(const hades::RGB &color);
    void clear(gl::ClearMask mask = gl::ClearMask::color | gl::ClearMask::depth);

    void flush();

    /***********
     * BATCHING
     */

    void new_batch_set(const std::string &name);
    void switch_to_batch_set(const std::string &name);

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

    std::unique_ptr<hades::CP437> load_cp437(const std::string &path, int char_w, int char_h, bool retro = false);

private:
    GladGLContext *ctx_{nullptr};

    std::unique_ptr<hades::ResourceLoader> resource_loader{nullptr};

    std::unordered_map<std::string, std::unique_ptr<BatchSet>> batch_sets_{};
    std::string active_batch_{"default"};
};

} // namespace hades

#endif //GL_CONTEXT_H
