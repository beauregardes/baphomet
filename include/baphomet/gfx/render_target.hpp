#pragma once

#include "baphomet/gfx/gl/framebuffer.hpp"
#include "baphomet/gfx/internal/batch_set.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <string>

namespace baphomet {

class RenderTarget {
  friend class GfxMgr;

public:
  RenderTarget(
      const std::string &tag,
      std::uint64_t weight,
      float x, float y, float w, float h
  );
  ~RenderTarget() = default;

  RenderTarget(const RenderTarget &) = delete;
  RenderTarget &operator=(const RenderTarget &) = delete;

  RenderTarget(RenderTarget &&other) noexcept = delete;
  RenderTarget &operator=(RenderTarget &&other) noexcept = delete;

  float x() const;
  float y() const;
  float w() const;
  float h() const;

  void resize(float x, float y, float w, float h);
  void resize(float w, float h);

private:
  std::string tag_;
  std::uint64_t weight_{0};
  float x_{0.0f}, y_{0.0f}, w_{0.0f}, h_{0.0f};

  std::unique_ptr<gl::Framebuffer> fbo_{nullptr};
  glm::mat4 projection_{1.0f};

  std::unique_ptr<BatchSet> batches_{nullptr};

  std::unique_ptr<gl::Shader> shader_{nullptr};
  std::unique_ptr<gl::VertexArray> vao_{nullptr};
  std::unique_ptr<gl::VecBuffer<float>> vbo_{nullptr};

  void draw_(glm::mat4 window_projection);
};

} // namespace baphomet
