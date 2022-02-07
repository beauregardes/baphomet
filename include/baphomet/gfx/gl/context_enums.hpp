#pragma once

#include "glad/gl.h"

#include "baphomet/util/enum_bitmask_ops.hpp"

namespace baphomet::gl {

enum class ClearMask {
  color = GL_COLOR_BUFFER_BIT,
  depth = GL_DEPTH_BUFFER_BIT,
  stencil = GL_STENCIL_BUFFER_BIT
};

enum class ClipOrigin {
  lower_left = GL_LOWER_LEFT,
  upper_left = GL_UPPER_LEFT
};

enum class ClipDepth {
  negative_one_to_one = GL_NEGATIVE_ONE_TO_ONE,
  zero_to_one = GL_ZERO_TO_ONE
};

enum class Capability {
  blend = GL_BLEND,
  depth_test = GL_DEPTH_TEST,
  scissor_test = GL_SCISSOR_TEST,
  stencil_test = GL_STENCIL_TEST,
};

enum class BlendFunc {
  zero = GL_ZERO,
  one = GL_ONE,
  src_color = GL_SRC_COLOR,
  one_minus_src_color = GL_ONE_MINUS_SRC_COLOR,
  dst_color = GL_DST_COLOR,
  one_minus_dst_color = GL_ONE_MINUS_DST_COLOR,
  src_alpha = GL_SRC_ALPHA,
  one_minus_src_alpha = GL_ONE_MINUS_SRC_ALPHA,
  dst_alpha = GL_DST_ALPHA,
  one_minus_dst_alpha = GL_ONE_MINUS_DST_ALPHA,
  constant_color = GL_CONSTANT_COLOR,
  one_minus_constant_color = GL_ONE_MINUS_CONSTANT_COLOR,
  constant_alpha = GL_CONSTANT_ALPHA,
  one_minus_constant_alpha = GL_ONE_MINUS_CONSTANT_ALPHA,
  src_alpha_saturate = GL_SRC_ALPHA_SATURATE,
  src1_color = GL_SRC1_COLOR,
  one_minus_src1_color = GL_ONE_MINUS_SRC1_COLOR,
  src1_alpha = GL_SRC1_ALPHA,
  one_minus_src1_alpha = GL_ONE_MINUS_SRC1_ALPHA
};

enum class DepthFunc {
  never = GL_NEVER,
  less = GL_LESS,
  equal = GL_EQUAL,
  lequal = GL_LEQUAL,
  greater = GL_GREATER,
  notequal = GL_NOTEQUAL,
  gequal = GL_GEQUAL,
  always = GL_ALWAYS
};

} // namespace baphomet::gl

ENABLE_BITMASK_OPERATORS(baphomet::gl::ClearMask)