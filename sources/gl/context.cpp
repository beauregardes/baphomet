#include "gl/context.hpp"

namespace gl {

Context::Context(GladGLContext *ctx, const std::string &tag)
    : tag(tag), ctx_(ctx) {

    pixels_ = std::make_unique<PixelBatch>(ctx_);
    lines_ = std::make_unique<LineBatch>(ctx_);
    tris_ = std::make_unique<TriBatch>(ctx_);
    rects_ = std::make_unique<RectBatch>(ctx_);
    ovals_ = std::make_unique<OvalBatch>(ctx_);
}

Context::Context(Context &&other) noexcept {
    ctx_ = other.ctx_;
    z_level_ = other.z_level_;
    pixels_ = std::move(other.pixels_);
    lines_ = std::move(other.lines_);
    tris_ = std::move(other.tris_);
    rects_ = std::move(other.rects_);
    ovals_ = std::move(other.ovals_);
    auto_clear_batches_ = other.auto_clear_batches_;

    other.ctx_ = nullptr;
}

Context &Context::operator=(Context &&other) noexcept {
    if (this != &other) {
        ctx_ = other.ctx_;
        z_level_ = other.z_level_;
        pixels_ = std::move(other.pixels_);
        lines_ = std::move(other.lines_);
        tris_ = std::move(other.tris_);
        rects_ = std::move(other.rects_);
        ovals_ = std::move(other.ovals_);
        auto_clear_batches_ = other.auto_clear_batches_;

        other.ctx_ = nullptr;
    }
    return *this;
}

bool Context::auto_clear_batches() {
    return auto_clear_batches_;
}

void Context::set_auto_clear_batches(bool s) {
    auto_clear_batches_ = s;
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

void Context::clear(ClearMask mask) {
    ctx_->Clear(unwrap(mask));
}

void Context::flush() {
    ctx_->Flush();
}

/*************
 * PRIMITIVES
 */

void Context::clear_batches() {
    pixels_->clear();
    lines_->clear();
    tris_->clear();
    rects_->clear();
    ovals_->clear();

    z_level_ = 1.0f;
}

void Context::pixel(float x, float y, const hades::RGB &color) {
    auto cv = color.vec4();
    pixels_->add(x + 0.5f, y + 0.5f, z_level_, cv.r, cv.g, cv.b, cv.a);
    z_level_++;
}

void Context::line(float x0, float y0, float x1, float y1, const hades::RGB &color, float cx, float cy, float angle) {
    auto cv = color.vec4();
    lines_->add(
        x0 + 0.5f, y0 + 0.5f,
        x1 + 0.5f, y1 + 0.5f,
        z_level_,
        cv.r, cv.g, cv.b, cv.a,
        cx, cy, glm::radians(angle)
    );
    z_level_++;
}

void Context::line(float x0, float y0, float x1, float y1, const hades::RGB &color, float angle) {
    line(x0, y0, x1, y1, color, (x0 + x1) / 2.0f, (y0 + y1) / 2.0f, angle);
}

void Context::line(float x0, float y0, float x1, float y1, const hades::RGB &color) {
    line(x0, y0, x1, y1, color, 0.0f, 0.0f, 0.0f);
}

void Context::tri(float x0, float y0, float x1, float y1, float x2, float y2, const hades::RGB &color, float cx, float cy, float angle) {
    auto cv = color.vec4();
    tris_->add(
        x0, y0,
        x1, y1,
        x2, y2,
        z_level_,
        cv.r, cv.g, cv.b, cv.a,
        cx, cy, glm::radians(angle)
    );
    z_level_++;
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
    rects_->add(
        x, y,
        w, h,
        z_level_,
        cv.r, cv.g, cv.b, cv.a,
        cx, cy, glm::radians(angle)
    );
    z_level_++;
}

void Context::rect(float x, float y, float w, float h, const hades::RGB &color, float angle) {
    rect(x, y, w, h, color, (x + w) / 2.0f, (y + h) / 2.0f, angle);
}

void Context::rect(float x, float y, float w, float h, const hades::RGB &color) {
    rect(x, y, w, h, color, 0.0f, 0.0f, 0.0f);
}

void Context::oval(float x, float y, float x_radius, float y_radius, const hades::RGB &color, float cx, float cy, float angle) {
    auto cv = color.vec4();
    ovals_->add(
        x + 0.5f, y + 0.5f,
        x_radius + 0.5f, y_radius + 0.5f,
        z_level_,
        cv.r, cv.g, cv.b, cv.a,
        cx, cy, angle
    );
    z_level_++;
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

void Context::draw_batches(glm::mat4 projection) {
    ovals_->draw(z_level_, projection);
    rects_->draw(z_level_, projection);
    tris_->draw(z_level_, projection);
    lines_->draw(z_level_, projection);
    pixels_->draw(z_level_, projection);
}

} // namespace gl