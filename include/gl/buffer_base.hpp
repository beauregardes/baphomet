#ifndef GL_BUFFERBASE_HPP
#define GL_BUFFERBASE_HPP

#include "glad/gl.h"

#include "hades/internal/bitmask_enum.hpp"

namespace gl {

enum class BufTarget {
  array = GL_ARRAY_BUFFER,
  atomic_counter = GL_ATOMIC_COUNTER_BUFFER,
  copy_read = GL_COPY_READ_BUFFER,
  copy_write = GL_COPY_WRITE_BUFFER,
  dispatch_indirect = GL_DISPATCH_INDIRECT_BUFFER,
  draw_indirect = GL_DRAW_INDIRECT_BUFFER,
  element_array = GL_ELEMENT_ARRAY_BUFFER,
  pixel_pack = GL_PIXEL_PACK_BUFFER,
  pixel_unpack = GL_PIXEL_UNPACK_BUFFER,
  query = GL_QUERY_BUFFER,
  shader_storage = GL_SHADER_STORAGE_BUFFER,
  texture = GL_TEXTURE_BUFFER,
  transform_feedback = GL_TRANSFORM_FEEDBACK_BUFFER,
  uniform = GL_UNIFORM_BUFFER,
  none = 0
};

enum class BufUsage {
  stream_draw = GL_STREAM_DRAW,
  stream_read = GL_STREAM_READ,
  stream_copy = GL_STREAM_COPY,
  static_draw = GL_STATIC_DRAW,
  static_read = GL_STATIC_READ,
  static_copy = GL_STATIC_COPY,
  dynamic_draw = GL_DYNAMIC_DRAW,
  dynamic_read = GL_DYNAMIC_READ,
  dynamic_copy = GL_DYNAMIC_COPY,
  none = 0
};

class BufferBase {
protected:
  GladGLContext *ctx_ {nullptr};

  void gen_id_();
  void del_id_();

public:
  GLuint id {0};

  BufferBase(GladGLContext *ctx);
  virtual ~BufferBase();

  // Copy constructors don't make sense for OpenGL objects
  BufferBase(const BufferBase &) = delete;
  BufferBase &operator=(const BufferBase &) = delete;

  BufferBase(BufferBase &&other) noexcept;
  BufferBase &operator=(BufferBase &&other) noexcept;

  void bind(BufTarget target) const;
  void unbind(BufTarget target) const;
};

} // namespace gl

#endif //GL_BUFFERBASE_HPP
