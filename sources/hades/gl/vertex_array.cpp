#include "hades/gl/vertex_array.hpp"
#include "spdlog/spdlog.h"

namespace gl {

VertexArray::VertexArray(GladGLContext *ctx)
    : ctx_(ctx) {
    gen_id_();
}

VertexArray::~VertexArray() {
    del_id_();
    ctx_ = nullptr;
}

VertexArray::VertexArray(VertexArray &&other) noexcept {
    id = other.id;
    ctx_ = other.ctx_;

    other.id = 0;
    other.ctx_ = nullptr;
}

VertexArray &VertexArray::operator=(VertexArray &&other) noexcept {
    if (this != &other) {
        del_id_();

        id = other.id;
        ctx_ = other.ctx_;

        other.id = 0;
        other.ctx_ = nullptr;
    }
    return *this;
}

void VertexArray::bind() {
    ctx_->BindVertexArray(id);
}

void VertexArray::unbind() {
    ctx_->BindVertexArray(0);
}

void VertexArray::attrib_pointer(const BufferBase *buffer, const std::vector<AttrDef> &definitions) {
    bind();
    buffer->bind(BufTarget::array);
    for (const auto &d : definitions)
        setup_enable_definition_(d);
    buffer->unbind(BufTarget::array);
    unbind();
}

void VertexArray::attrib_pointer(const BufferBase *buffer, const AttrDef &definition) {
    bind();
    buffer->bind(BufTarget::array);
    setup_enable_definition_(definition);
    buffer->unbind(BufTarget::array);
    unbind();
}

void VertexArray::draw_arrays(DrawMode mode, GLint first, GLsizei count) {
    bind();
    ctx_->DrawArrays(unwrap(mode), first, count);
    unbind();
}

void VertexArray::setup_enable_definition_(const AttrDef &definition) {
    ctx_->VertexAttribPointer(
        definition.index,
        definition.size,
        unwrap(definition.type),
        definition.normalized ? GL_TRUE : GL_FALSE,
        definition.stride,
        reinterpret_cast<void *>(definition.offset));
    ctx_->EnableVertexAttribArray(definition.index);
    spdlog::debug("VertexAttrib: {}, {}, {}, {}, {}, {}", definition.index, definition.size, unwrap(definition.type), definition.normalized, definition.stride, definition.offset);
}

void VertexArray::gen_id_() {
    ctx_->GenVertexArrays(1, &id);
    spdlog::debug("Generated vertex array ({})", id);
}

void VertexArray::del_id_() {
    if (ctx_ && id != 0) {
        ctx_->DeleteVertexArrays(1, &id);
        spdlog::debug("Deleted vertex array ({})", id);
    }
}

}  // namespace gl