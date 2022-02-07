#pragma once

#include "glad/gl.h"
#include "glm/glm.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace baphomet::gl {

class Shader {
public:
  GLuint id{0};
  std::string tag{};

  Shader() = default;
  ~Shader();

  // Copy constructors don't make sense for OpenGL objects
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  Shader(Shader &&other) noexcept;
  Shader &operator=(Shader &&other) noexcept;

  void use();

  void uniform_1f(const std::string &name, float v);
  void uniform_2f(const std::string &name, glm::vec2 v);
  void uniform_3f(const std::string &name, glm::vec3 v);
  void uniform_4f(const std::string &name, glm::vec4 v);

  void uniform_1d(const std::string &name, double v);
  void uniform_2d(const std::string &name, glm::dvec2 v);
  void uniform_3d(const std::string &name, glm::dvec3 v);
  void uniform_4d(const std::string &name, glm::dvec4 v);

  void uniform_1i(const std::string &name, int v);
  void uniform_2i(const std::string &name, glm::ivec2 v);
  void uniform_3i(const std::string &name, glm::ivec3 v);
  void uniform_4i(const std::string &name, glm::ivec4 v);

  void uniform_1ui(const std::string &name, unsigned int v);
  void uniform_2ui(const std::string &name, glm::uvec2 v);
  void uniform_3ui(const std::string &name, glm::uvec3 v);
  void uniform_4ui(const std::string &name, glm::uvec4 v);

  void uniform_1b(const std::string &name, bool v);
  void uniform_2b(const std::string &name, glm::bvec2 v);
  void uniform_3b(const std::string &name, glm::bvec3 v);
  void uniform_4b(const std::string &name, glm::bvec4 v);

  void uniform_mat2f(const std::string &name, glm::mat2 v, bool transpose = false);
  void uniform_mat3f(const std::string &name, glm::mat3 v, bool transpose = false);
  void uniform_mat4f(const std::string &name, glm::mat4 v, bool transpose = false);

  void uniform_mat2d(const std::string &name, glm::dmat2 v, bool transpose = false);
  void uniform_mat3d(const std::string &name, glm::dmat3 v, bool transpose = false);
  void uniform_mat4d(const std::string &name, glm::dmat4 v, bool transpose = false);

  void uniform_mat2x3f(const std::string &name, glm::mat2x3 v, bool transpose = false);
  void uniform_mat3x2f(const std::string &name, glm::mat3x2 v, bool transpose = false);
  void uniform_mat2x4f(const std::string &name, glm::mat2x4 v, bool transpose = false);
  void uniform_mat4x2f(const std::string &name, glm::mat4x2 v, bool transpose = false);
  void uniform_mat3x4f(const std::string &name, glm::mat3x4 v, bool transpose = false);
  void uniform_mat4x3f(const std::string &name, glm::mat4x3 v, bool transpose = false);

  void uniform_mat2x3d(const std::string &name, glm::dmat2x3 v, bool transpose = false);
  void uniform_mat3x2d(const std::string &name, glm::dmat3x2 v, bool transpose = false);
  void uniform_mat2x4d(const std::string &name, glm::dmat2x4 v, bool transpose = false);
  void uniform_mat4x2d(const std::string &name, glm::dmat4x2 v, bool transpose = false);
  void uniform_mat3x4d(const std::string &name, glm::dmat3x4 v, bool transpose = false);
  void uniform_mat4x3d(const std::string &name, glm::dmat4x3 v, bool transpose = false);

private:
  std::unordered_map<std::string, GLint> attrib_locs_{};
  std::unordered_map<std::string, GLint> uniform_locs_{};

  GLint get_attrib_loc_(const std::string &name);
  GLint get_uniform_loc_(const std::string &name);

  void del_id_();
};

class ShaderBuilder {
public:
  ShaderBuilder();
  ShaderBuilder(const std::string &tag);

  ~ShaderBuilder() = default;

  ShaderBuilder(const ShaderBuilder &) = delete;
  ShaderBuilder &operator=(const ShaderBuilder &) = delete;

  ShaderBuilder(ShaderBuilder &&) noexcept = delete;
  ShaderBuilder &operator=(ShaderBuilder &&) noexcept = delete;

  ShaderBuilder &vert_from_src(const std::string &src);
  ShaderBuilder &vert_from_file(const std::string &path);

  ShaderBuilder &frag_from_src(const std::string &src);
  ShaderBuilder &frag_from_file(const std::string &path);

  ShaderBuilder &varyings(const std::vector<std::string> &vs);

  std::unique_ptr<Shader> link();

private:
  GLuint vert_id_{0u};
  GLuint frag_id_{0u};
  GLuint program_id_{0u};

  std::string tag_{};

  bool check_compile_(GLuint shader_id, GLenum type);

  bool check_link_();

  std::string read_file_(const std::string &path);
};

} // namespace baphomet::gl