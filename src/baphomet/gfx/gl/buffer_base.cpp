#include "baphomet/gfx/gl/buffer_base.hpp"

#include "baphomet/util/enum_bitmask_ops.hpp"

#include "spdlog/spdlog.h"

namespace baphomet::gl {

BufferBase::~BufferBase() {
  del_id_();
}

BufferBase::BufferBase(BufferBase &&other) noexcept {
  std::swap(id, other.id);
}

BufferBase &BufferBase::operator=(BufferBase &&other) noexcept {
  if (this != &other) {
    del_id_();

    std::swap(id, other.id);
  }
  return *this;
}

void BufferBase::bind(BufTarget target) const {
  glBindBuffer(unwrap(target), id);
}

void BufferBase::unbind(BufTarget target) const {
  glBindBuffer(unwrap(target), 0);
}

void BufferBase::gen_id_() {
  glGenBuffers(1, &id);
  spdlog::trace("Generated buffer ({})", id);
}

void BufferBase::del_id_() {
  if (id != 0) {
    glDeleteBuffers(1, &id);
    spdlog::trace("Deleted buffer ({})", id);
  }
}

} // namespace baphomet::gl