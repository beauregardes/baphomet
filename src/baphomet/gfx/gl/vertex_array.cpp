#include "baphomet/gfx/gl/vertex_array.hpp"

#include "baphomet/util/enum_bitmask_ops.hpp"

#include "spdlog/spdlog.h"

namespace baphomet::gl {

VertexArray::VertexArray() {
  gen_id_();
}

VertexArray::~VertexArray() {
  del_id_();
}

VertexArray::VertexArray(VertexArray &&other) noexcept {
  std::swap(id, other.id);
}

VertexArray &VertexArray::operator=(VertexArray &&other) noexcept {
  if (this != &other) {
    del_id_();
    std::swap(id, other.id);
  }
  return *this;
}

void VertexArray::bind() {
  glBindVertexArray(id);
}

void VertexArray::unbind() {
  glBindVertexArray(0);
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

void VertexArray::indices(const BufferBase *buffer) {
  bind();
  buffer->bind(BufTarget::element_array);
  unbind();
}

void VertexArray::draw_arrays(DrawMode mode, GLint first, GLsizei count) {
  bind();
  glDrawArrays(unwrap(mode), first, count);
  unbind();
}

void VertexArray::draw_elements(DrawMode mode, GLsizei count, GLenum type, void *indices) {
  bind();
  glDrawElements(unwrap(mode), count, type, indices);
  unbind();
}

void VertexArray::setup_enable_definition_(const AttrDef &definition) {
  glVertexAttribPointer(
      definition.index,
      definition.size,
      unwrap(definition.type),
      definition.normalized ? GL_TRUE : GL_FALSE,
      definition.stride,
      reinterpret_cast<void *>(definition.offset));
  glEnableVertexAttribArray(definition.index);
  spdlog::trace("VertexAttrib: {}, {}, {}, {}, {}, {}", definition.index, definition.size, unwrap(definition.type), definition.normalized, definition.stride, definition.offset);
}

void VertexArray::gen_id_() {
  glGenVertexArrays(1, &id);
  spdlog::trace("Generated vertex array ({})", id);
}

void VertexArray::del_id_() {
  if (id != 0) {
    glDeleteVertexArrays(1, &id);
    spdlog::trace("Deleted vertex array ({})", id);
  }
}

}  // namespace baphomet::gl
