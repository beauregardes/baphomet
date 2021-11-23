#include "hades/context.hpp"

#include "hades/util/random.hpp"

namespace hades {

Context::Context(GladGLContext *ctx, const std::string &tag)
    : tag(tag), ctx_(ctx) {

    resource_loader = std::make_unique<hades::ResourceLoader>(ctx);

    batch_sets_[active_batch_] = std::make_unique<BatchSet>(ctx);
}

Context::Context(Context &&other) noexcept {
    ctx_ = other.ctx_;
    resource_loader = std::move(other.resource_loader);
    batch_sets_ = std::move(other.batch_sets_);
    active_batch_ = other.active_batch_;

    other.ctx_ = nullptr;
    other.active_batch_ = "default";
}

Context &Context::operator=(Context &&other) noexcept {
    if (this != &other) {
        ctx_ = other.ctx_;
        resource_loader = std::move(other.resource_loader);
        batch_sets_ = std::move(other.batch_sets_);
        active_batch_ = other.active_batch_;

        other.ctx_ = nullptr;
        other.active_batch_ = "default";
    }
    return *this;
}

void Context::enable(gl::Capability cap) {
    ctx_->Enable(unwrap(cap));
}

void Context::disable(gl::Capability cap) {
    ctx_->Disable(unwrap(cap));
}

void Context::depth_func(gl::DepthFunc func) {
    ctx_->DepthFunc(unwrap(func));
}

void Context::depth_mask(bool flag) {
    ctx_->DepthMask(flag ? GL_TRUE : GL_FALSE);
}

void Context::blend_func(gl::BlendFunc src, gl::BlendFunc dst) {
    ctx_->BlendFunc(unwrap(src), unwrap(dst));
}

void Context::viewport(int x, int y, int w, int h) {
    ctx_->Viewport(x, y, w, h);
}

void Context::clip_control(gl::ClipOrigin origin, gl::ClipDepth depth) {
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

void Context::clear(gl::ClearMask mask) {
    ctx_->Clear(unwrap(mask));
}

void Context::flush() {
    ctx_->Flush();
    clear(gl::ClearMask::depth);
}

/***********
 * BATCHING
 */

void Context::new_batch_set(const std::string &name) {
    batch_sets_[name] = std::make_unique<BatchSet>(ctx_);
}

void Context::switch_to_batch_set(const std::string &name) {
    active_batch_ = name;
}

void Context::clear_batches() {
    batch_sets_[active_batch_]->clear();
}

void Context::draw_batches(glm::mat4 projection) {
    batch_sets_[active_batch_]->draw_opaque(projection);

    enable(gl::Capability::blend);
    depth_mask(false);

    batch_sets_[active_batch_]->draw_alpha(projection);
    
    depth_mask(true);
    disable(gl::Capability::blend);
}

/*************
 * PRIMITIVES
 */

void Context::pixel(float x, float y, const hades::RGB &color) {
    auto cv = color.vec4();
    batch_sets_[active_batch_]->pixels->add(x + 0.5f, y + 0.5f, batch_sets_[active_batch_]->z_level, cv.r, cv.g, cv.b, cv.a);
    batch_sets_[active_batch_]->z_level++;
}

void Context::line(float x0, float y0, float x1, float y1, const hades::RGB &color, float cx, float cy, float angle) {
    auto cv = color.vec4();
    batch_sets_[active_batch_]->lines->add(
        x0 + 0.5f, y0 + 0.5f,
        x1 + 0.5f, y1 + 0.5f,
        batch_sets_[active_batch_]->z_level,
        cv.r, cv.g, cv.b, cv.a,
        cx, cy, glm::radians(angle)
    );
    batch_sets_[active_batch_]->z_level++;
}

void Context::line(float x0, float y0, float x1, float y1, const hades::RGB &color, float angle) {
    line(x0, y0, x1, y1, color, (x0 + x1) / 2.0f, (y0 + y1) / 2.0f, angle);
}

void Context::line(float x0, float y0, float x1, float y1, const hades::RGB &color) {
    line(x0, y0, x1, y1, color, 0.0f, 0.0f, 0.0f);
}

void Context::tri(float x0, float y0, float x1, float y1, float x2, float y2, const hades::RGB &color, float cx, float cy, float angle) {
    auto cv = color.vec4();
    batch_sets_[active_batch_]->tris->add(
        x0, y0,
        x1, y1,
        x2, y2,
        batch_sets_[active_batch_]->z_level,
        cv.r, cv.g, cv.b, cv.a,
        cx, cy, glm::radians(angle)
    );
    batch_sets_[active_batch_]->z_level++;
}

void Context::tri(float x0, float y0, float x1, float y1, float x2, float y2, const hades::RGB &color, float angle) {
    tri(x0, y0, x1, y1, x2, y2, color, (x0 + x1 + x2) / 3.0f, (y0 + y1 + y2) / 3.0f, angle);
}

void Context::tri(float x0, float y0, float x1, float y1, float x2, float y2, const hades::RGB &color) {
    tri(x0, y0, x1, y1, x2, y2, color, 0.0f, 0.0f, 0.0f);
}

void Context::tri(float x, float y, float radius, const hades::RGB &color, float angle) {
    tri(
        x + radius * std::cos(-glm::radians(90.0f)),  y + radius * std::sin(-glm::radians(90.0f)),
        x + radius * std::cos(-glm::radians(210.0f)), y + radius * std::sin(-glm::radians(210.0f)),
        x + radius * std::cos(-glm::radians(330.0f)), y + radius * std::sin(-glm::radians(330.0f)),
        color,
        x, y, angle
    );
}

void Context::tri(float x, float y, float radius, const hades::RGB &color) {
    tri(
        x + radius * std::cos(-glm::radians(90.0f)),  y + radius * std::sin(-glm::radians(90.0f)),
        x + radius * std::cos(-glm::radians(210.0f)), y + radius * std::sin(-glm::radians(210.0f)),
        x + radius * std::cos(-glm::radians(330.0f)), y + radius * std::sin(-glm::radians(330.0f)),
        color,
        0.0f, 0.0f, 0.0f
    );
}

void Context::rect(float x, float y, float w, float h, const hades::RGB &color, float cx, float cy, float angle) {
    auto cv = color.vec4();
    batch_sets_[active_batch_]->rects->add(
        x, y,
        w, h,
        batch_sets_[active_batch_]->z_level,
        cv.r, cv.g, cv.b, cv.a,
        cx, cy, glm::radians(angle)
    );
    batch_sets_[active_batch_]->z_level++;
}

void Context::rect(float x, float y, float w, float h, const hades::RGB &color, float angle) {
    rect(x, y, w, h, color, (x + w) / 2.0f, (y + h) / 2.0f, angle);
}

void Context::rect(float x, float y, float w, float h, const hades::RGB &color) {
    rect(x, y, w, h, color, 0.0f, 0.0f, 0.0f);
}

void Context::oval(float x, float y, float x_radius, float y_radius, const hades::RGB &color, float cx, float cy, float angle) {
    auto cv = color.vec4();
    batch_sets_[active_batch_]->ovals->add(
        x + 0.5f, y + 0.5f,
        x_radius + 0.5f, y_radius + 0.5f,
        batch_sets_[active_batch_]->z_level,
        cv.r, cv.g, cv.b, cv.a,
        cx, cy, angle
    );
    batch_sets_[active_batch_]->z_level++;
}

void Context::oval(float x, float y, float x_radius, float y_radius, const hades::RGB &color, float angle) {
    oval(x, y, x_radius, y_radius, color, x, y, angle);
}

void Context::oval(float x, float y, float x_radius, float y_radius, const hades::RGB &color) {
    oval(x, y, x_radius, y_radius, color, 0.0f, 0.0f, 0.0f);
}

void Context::circle(float x, float y, float radius, const hades::RGB &color, float cx, float cy, float angle) {
    oval(x, y, radius, radius, color, cx, cy, angle);
}

void Context::circle(float x, float y, float radius, const hades::RGB &color, float angle) {
    oval(x, y, radius, radius, color, x, y, 0.0f);
}

void Context::circle(float x, float y, float radius, const hades::RGB &color) {
    oval(x, y, radius, radius, color, 0.0f, 0.0f, 0.0f);
}

/***********
 * TEXTURES
 */

std::unique_ptr<hades::Texture> Context::load_texture(const std::string &path, bool retro) {
    auto name = hades::rand::base58(11);
    resource_loader->load_texture_unit(name, path, retro);

    auto &tex = resource_loader->get_texture_unit(name);
    batch_sets_[active_batch_]->textures[name] = std::make_unique<gl::TextureBatch>(ctx_, tex);

    return std::make_unique<hades::Texture>(
        batch_sets_[active_batch_]->textures[name],
        tex->width(), tex->height(),
        batch_sets_[active_batch_]->z_level
    );
}

std::unique_ptr<hades::CP437> Context::load_cp437(const std::string &path, int char_w, int char_h, bool retro) {
    auto name = hades::rand::base58(11);
    resource_loader->load_texture_unit(name, path, retro);

    auto &tex = resource_loader->get_texture_unit(name);
    batch_sets_[active_batch_]->textures[name] = std::make_unique<gl::TextureBatch>(ctx_, tex);

    return std::make_unique<hades::CP437>(
        batch_sets_[active_batch_]->textures[name],
        tex->width(), tex->height(),
        char_w, char_h,
        batch_sets_[active_batch_]->z_level
    );
}

} // namespace hades