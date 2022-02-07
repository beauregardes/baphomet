#include "baphomet/gfx/gl/framebuffer.hpp"

#include "baphomet/util/enum_bitmask_ops.hpp"
#include "baphomet/util/random.hpp"

#include "spdlog/spdlog.h"

namespace baphomet::gl {

Framebuffer::~Framebuffer() {
  del_id_();
}

Framebuffer::Framebuffer(Framebuffer &&other) noexcept {
  id = other.id;
  width = other.width;
  height = other.height;
  tex_attachments_ = std::move(other.tex_attachments_);
  rbo_attachments_ = std::move(other.rbo_attachments_);

  other.id = 0;
  other.width = 0;
  other.height = 0;
  other.tex_attachments_.clear();
  other.rbo_attachments_.clear();
}

Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept {
  if (this != &other) {
    del_id_();

    id = other.id;
    width = other.width;
    height = other.height;
    tex_attachments_ = std::move(other.tex_attachments_);
    rbo_attachments_ = std::move(other.rbo_attachments_);

    other.id = 0;
    other.width = 0;
    other.height = 0;
    other.tex_attachments_.clear();
    other.rbo_attachments_.clear();
  }
  return *this;
}

void Framebuffer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, id);
  glViewport(0, 0, width, height);
}

void Framebuffer::bind(FboTarget target) {
  glBindFramebuffer(unwrap(target), id);
}

void Framebuffer::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::copy_to_default_framebuffer(bool retro) {
  bind(FboTarget::read);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(
      0, 0, width, height,
      0, 0, width, height,
      GL_COLOR_BUFFER_BIT, retro ? GL_NEAREST : GL_LINEAR
  );
  glBindFramebuffer(unwrap(FboTarget::read), 0);
  glBindFramebuffer(unwrap(FboTarget::draw), 0);
}

void Framebuffer::copy_to_default_framebuffer(GLint window_width, GLint window_height, bool retro) {
  bind(FboTarget::read);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(
      0, 0, width, height,
      0, 0, window_width, window_height,
      GL_COLOR_BUFFER_BIT, retro ? GL_NEAREST : GL_LINEAR
  );
  glBindFramebuffer(unwrap(FboTarget::read), 0);
  glBindFramebuffer(unwrap(FboTarget::draw), 0);
}

void Framebuffer::del_id_() {
  if (id != 0) {
    spdlog::trace("Deleting framebuffer ({})", id);
    for (auto &p: tex_attachments_) {
      spdlog::trace("FBO/Deleting texture ({} / {})", p.second, id);
      glDeleteTextures(1, &p.second);
    }
    for (auto &p: rbo_attachments_) {
      spdlog::trace("FBO/Deleting renderbuffer ({} / {})", p.second, id);
      glDeleteRenderbuffers(1, &p.second);
    }
    glDeleteFramebuffers(1, &id);
  }
}

FramebufferBuilder::FramebufferBuilder(GLsizei width, GLsizei height)
    : width_(width), height_(height) {
  gen_id_();
  glBindFramebuffer(GL_FRAMEBUFFER, id_);
}

FramebufferBuilder &FramebufferBuilder::texture(const std::string &tag, TexFormat internalformat, bool retro) {
  GLuint tex;
  glGenTextures(1, &tex);
  spdlog::trace("FBO/Generated texture ({})", tex);

  glBindTexture(GL_TEXTURE_2D, tex);
  glTexStorage2D(GL_TEXTURE_2D, 1, unwrap(internalformat), width_, height_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, retro ? GL_NEAREST : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, retro ? GL_NEAREST : GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D,
      tex,
      0
  );

  tex_attachments_[tag] = tex;

  return *this;
}

FramebufferBuilder &FramebufferBuilder::texture(TexFormat format) {
  return texture(rnd::base58(11), format);
}

FramebufferBuilder &FramebufferBuilder::renderbuffer(const std::string &tag, RBufFormat internalformat) {
  GLuint rbo;
  glGenRenderbuffers(1, &rbo);
  spdlog::trace("FBO/Generated renderbuffer ({})", rbo);

  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, unwrap(internalformat), width_, height_);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(
      GL_FRAMEBUFFER,
      get_renderbuffer_attachment_type(unwrap(internalformat)),
      GL_RENDERBUFFER,
      rbo
  );

  rbo_attachments_[tag] = rbo;

  return *this;
}

FramebufferBuilder &FramebufferBuilder::renderbuffer(RBufFormat internalformat) {
  return renderbuffer(rnd::base58(11), internalformat);
}

std::unique_ptr<Framebuffer> FramebufferBuilder::check_complete() {
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::error("Framebuffer is not complete");
  else
    spdlog::trace("Completed framebuffer ({})", id_);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  auto f = std::make_unique<Framebuffer>();
  f->id = id_;
  f->width = width_;
  f->height = height_;
  f->tex_attachments_ = std::move(tex_attachments_);
  f->rbo_attachments_ = std::move(rbo_attachments_);

  return f;
}

void FramebufferBuilder::gen_id_() {
  glGenFramebuffers(1, &id_);
  spdlog::trace("Generated framebuffer ({})", id_);
}

GLenum FramebufferBuilder::get_texture_format(GLenum internalformat) {
  switch (internalformat) {
    case GL_R8:
    case GL_R8_SNORM:
    case GL_R16:
    case GL_R16_SNORM:
    case GL_R16F:
    case GL_R32F:
    case GL_R8I:
    case GL_R8UI:
    case GL_R16I:
    case GL_R16UI:
    case GL_R32I:
    case GL_R32UI:
    case GL_COMPRESSED_RED_RGTC1:
    case GL_COMPRESSED_SIGNED_RED_RGTC1:
    case GL_COMPRESSED_R11_EAC:
    case GL_COMPRESSED_SIGNED_R11_EAC:
      return GL_RED;

    case GL_RG8:
    case GL_RG8_SNORM:
    case GL_RG16:
    case GL_RG16_SNORM:
    case GL_RG16F:
    case GL_RG32F:
    case GL_RG8I:
    case GL_RG8UI:
    case GL_RG16I:
    case GL_RG16UI:
    case GL_RG32I:
    case GL_RG32UI:
    case GL_COMPRESSED_RG_RGTC2:
    case GL_COMPRESSED_SIGNED_RG_RGTC2:
    case GL_COMPRESSED_RG11_EAC:
    case GL_COMPRESSED_SIGNED_RG11_EAC:
      return GL_RG;

    case GL_R3_G3_B2:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB565:
    case GL_RGB8:
    case GL_RGB8_SNORM:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
    case GL_RGB16_SNORM:
    case GL_SRGB8:
    case GL_RGB16F:
    case GL_RGB32F:
    case GL_R11F_G11F_B10F:
    case GL_RGB9_E5:
    case GL_RGB8I:
    case GL_RGB8UI:
    case GL_RGB16I:
    case GL_RGB16UI:
    case GL_RGB32I:
    case GL_RGB32UI:
    case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB:
    case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB:
    case GL_COMPRESSED_RGB8_ETC2:
    case GL_COMPRESSED_SRGB8_ETC2:
    case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
      return GL_RGB;

    case GL_RGBA2:
    case GL_RGBA4:
    case GL_RGB5_A1:
    case GL_RGBA8:
    case GL_RGBA8_SNORM:
    case GL_RGB10_A2:
    case GL_RGB10_A2UI:
    case GL_RGBA12:
    case GL_RGBA16:
    case GL_RGBA16_SNORM:
    case GL_SRGB8_ALPHA8:
    case GL_RGBA16F:
    case GL_RGBA32F:
    case GL_RGBA8I:
    case GL_RGBA8UI:
    case GL_RGBA16I:
    case GL_RGBA16UI:
    case GL_RGBA32I:
    case GL_RGBA32UI:
    case GL_COMPRESSED_RGBA_BPTC_UNORM_ARB:
    case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB:
    case GL_COMPRESSED_RGBA8_ETC2_EAC:
    case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
      return GL_RGBA;

    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32F:
      return GL_DEPTH_COMPONENT;

    case GL_DEPTH24_STENCIL8:
    case GL_DEPTH32F_STENCIL8:
      return GL_DEPTH_STENCIL;

    case GL_STENCIL_INDEX8:
      return GL_STENCIL_INDEX;

    default:
      spdlog::error("Bad value given to get_texture_format: {}", internalformat);
      return GL_RGBA;
  }
}

GLenum FramebufferBuilder::get_renderbuffer_attachment_type(GLenum internalformat) {
  switch (internalformat) {
    case GL_STENCIL_INDEX8:
      return GL_STENCIL_ATTACHMENT;

    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32F:
      return GL_DEPTH_ATTACHMENT;

    case GL_DEPTH24_STENCIL8:
    case GL_DEPTH32F_STENCIL8:
      return GL_DEPTH_STENCIL_ATTACHMENT;

    default:
      return GL_COLOR_ATTACHMENT0;
  }
}

} // namespace baphomet::gl
