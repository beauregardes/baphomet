#include "horns/shader.hpp"

#include "goat/util/rnd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "spdlog/spdlog.h"
#include <fstream>

namespace horns {

Shader::~Shader() {
  del_id_();
}

Shader::Shader(Shader &&other) noexcept {
  id = other.id;
  tag = other.tag;
  attrib_locs_ = other.attrib_locs_;
  uniform_locs_ = other.uniform_locs_;

  other.id = 0;
  other.tag = "";
  other.attrib_locs_.clear();
  other.uniform_locs_.clear();
}

Shader &Shader::operator=(Shader &&other) noexcept {
  if (this != &other) {
    del_id_();

    id = other.id;
    tag = other.tag;
    attrib_locs_ = other.attrib_locs_;
    uniform_locs_ = other.uniform_locs_;

    other.id = 0;
    other.tag = "";
    other.attrib_locs_.clear();
    other.uniform_locs_.clear();
  }
  return *this;
}

void Shader::use() {
  glUseProgram(id);
}

void Shader::uniform_1f(const std::string &name, float v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform1f(loc, v);
}

void Shader::uniform_2f(const std::string &name, glm::vec2 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform2fv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_3f(const std::string &name, glm::vec3 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform3fv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_4f(const std::string &name, glm::vec4 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform4fv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_1d(const std::string &name, double v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform1d(loc, v);
}

void Shader::uniform_2d(const std::string &name, glm::dvec2 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform2dv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_3d(const std::string &name, glm::dvec3 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform3dv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_4d(const std::string &name, glm::dvec4 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform4dv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_1i(const std::string &name, int v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform1i(loc, v);
}

void Shader::uniform_2i(const std::string &name, glm::ivec2 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform2iv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_3i(const std::string &name, glm::ivec3 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform3iv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_4i(const std::string &name, glm::ivec4 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform4iv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_1ui(const std::string &name, unsigned int v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform1ui(loc, v);
}

void Shader::uniform_2ui(const std::string &name, glm::uvec2 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform2uiv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_3ui(const std::string &name, glm::uvec3 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform3uiv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_4ui(const std::string &name, glm::uvec4 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform4uiv(loc, 1, glm::value_ptr(v));
}

void Shader::uniform_1b(const std::string &name, bool v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniform1i(loc, v ? 1 : 0);
}

void Shader::uniform_2b(const std::string &name, glm::bvec2 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1) {
    const int iv[2] = {
        v[0] ? 1 : 0,
        v[1] ? 1 : 0
    };
    glUniform2iv(loc, 1, iv);
  }
}

void Shader::uniform_3b(const std::string &name, glm::bvec3 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1) {
    const int iv[3] = {
        v[0] ? 1 : 0,
        v[1] ? 1 : 0,
        v[2] ? 1 : 0
    };
    glUniform3iv(loc, 1, iv);
  }
}

void Shader::uniform_4b(const std::string &name, glm::bvec4 v) {
  int loc = get_uniform_loc_(name);
  if (loc != -1) {
    const int iv[4] = {
        v[0] ? 1 : 0,
        v[1] ? 1 : 0,
        v[2] ? 1 : 0,
        v[3] ? 1 : 0
    };
    glUniform3iv(loc, 1, iv);
  }
}

void Shader::uniform_mat2f(const std::string &name, glm::mat2 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix2fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat3f(const std::string &name, glm::mat3 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix3fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat4f(const std::string &name, glm::mat4 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix4fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat2d(const std::string &name, glm::dmat2 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix2dv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat3d(const std::string &name, glm::dmat3 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix3dv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat4d(const std::string &name, glm::dmat4 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix4dv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat2x3f(const std::string &name, glm::mat2x3 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix2x3fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat3x2f(const std::string &name, glm::mat3x2 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix3x2fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat2x4f(const std::string &name, glm::mat2x4 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix2x4fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat4x2f(const std::string &name, glm::mat4x2 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix4x2fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat3x4f(const std::string &name, glm::mat3x4 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix3x4fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat4x3f(const std::string &name, glm::mat4x3 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix4x3fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat2x3d(const std::string &name, glm::dmat2x3 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix2x3dv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat3x2d(const std::string &name, glm::dmat3x2 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix3x2dv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat2x4d(const std::string &name, glm::dmat2x4 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix2x4dv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat4x2d(const std::string &name, glm::dmat4x2 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix4x2dv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat3x4d(const std::string &name, glm::dmat3x4 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix3x4dv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

void Shader::uniform_mat4x3d(const std::string &name, glm::dmat4x3 v, bool transpose) {
  int loc = get_uniform_loc_(name);
  if (loc != -1)
    glUniformMatrix4x3dv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(v));
}

GLint Shader::get_attrib_loc_(const std::string &name) {
  if (!attrib_locs_.contains(name)) {
    GLint loc = glGetAttribLocation(id, name.c_str());
    if (loc == -1)
      spdlog::error("Failed to get location of attrib: '{}'", name);
    else
      spdlog::trace("Located attrib '{}' in shader ({} / {}) at loc {}", name, id, tag, loc);
    attrib_locs_[name] = loc;
  }
  return attrib_locs_[name];
}

GLint Shader::get_uniform_loc_(const std::string &name) {
  if (!uniform_locs_.contains(name)) {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc == -1)
      spdlog::error("Failed to get location of uniform: '{}'", name);
    else
      spdlog::trace("Located uniform '{}' in shader ({} / {}) at loc {}", name, id, tag, loc);
    uniform_locs_[name] = loc;
  }
  return uniform_locs_[name];
}

void Shader::del_id_() {
  if (id != 0) {
    glDeleteProgram(id);
    spdlog::trace("Deleted shader ({} / {})", id, tag);
  }
}

ShaderBuilder::ShaderBuilder()
    : ShaderBuilder(rnd::base58(11)) {}

ShaderBuilder::ShaderBuilder(const std::string &tag)
    : tag_(tag) {
  program_id_ = glCreateProgram();

  spdlog::trace("Generated shader ({} / {})", program_id_, tag_);
}

ShaderBuilder &ShaderBuilder::vert_from_src(const std::string &src) {
  vert_id_ = glCreateShader(GL_VERTEX_SHADER);

  const char *src_p = src.c_str();
  glShaderSource(vert_id_, 1, &src_p, nullptr);
  glCompileShader(vert_id_);

  if (check_compile_(vert_id_, GL_VERTEX_SHADER)) {
    glAttachShader(program_id_, vert_id_);
    spdlog::trace("Attached vertex shader ({})", tag_);
  }

  return *this;
}

ShaderBuilder &ShaderBuilder::vert_from_file(const std::string &path) {
  return vert_from_src(read_file_(path));
}

ShaderBuilder &ShaderBuilder::frag_from_src(const std::string &src) {
  frag_id_ = glCreateShader(GL_FRAGMENT_SHADER);

  const char *src_p = src.c_str();
  glShaderSource(frag_id_, 1, &src_p, nullptr);
  glCompileShader(frag_id_);

  if (check_compile_(frag_id_, GL_FRAGMENT_SHADER)) {
    glAttachShader(program_id_, frag_id_);
    spdlog::trace("Attached fragment shader ({})", tag_);
  }

  return *this;
}

ShaderBuilder &ShaderBuilder::frag_from_file(const std::string &path) {
  return frag_from_src(read_file_(path));
}

ShaderBuilder &ShaderBuilder::varyings(const std::vector<std::string> &vs) {
  if (!vs.empty()) {
    std::vector<const GLchar *> cs_varyings;

    // Reserve to avoid reallocation, which would invalidate the pointers
    cs_varyings.reserve(vs.size());
    for (const auto &v: vs)
      cs_varyings.push_back(v.c_str());

    glTransformFeedbackVaryings(
        program_id_,
        static_cast<GLsizei>(vs.size()),
        &cs_varyings[0],
        GL_INTERLEAVED_ATTRIBS
    );

    spdlog::trace("Setup varyings ({})", tag_);
  }

  return *this;
}

std::unique_ptr<Shader> ShaderBuilder::link() {
  glLinkProgram(program_id_);
  if (check_link_())
    spdlog::trace("Linked shader program ({})", tag_);

  if (vert_id_ != 0)
    glDeleteShader(vert_id_);

  if (frag_id_ != 0)
    glDeleteShader(frag_id_);

  auto s = std::make_unique<Shader>();
  s->id = program_id_;
  s->tag = tag_;

  return s;
}

bool ShaderBuilder::check_compile_(GLuint shader_id, GLenum type) {
  static auto info_log = std::vector<char>();
  static int success;

  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    info_log.clear();
    info_log.resize(512);
    glGetShaderInfoLog(
        shader_id,
        static_cast<GLsizei>(info_log.size()),
        nullptr,
        &info_log[0]
    );

    std::string type_str;
    if (type == GL_VERTEX_SHADER)
      type_str = "vertex shader_";
    else
      type_str = "fragment shader_";

    spdlog::error("Failed to compile {} ({})! Info log:\n{}", type_str, tag_, &info_log[0]);
    return false;
  }

  return true;
}

bool ShaderBuilder::check_link_() {
  static auto info_log = std::vector<char>();
  static int success;

  glGetProgramiv(program_id_, GL_LINK_STATUS, &success);
  if (!success) {
    info_log.clear();
    info_log.resize(512);
    glGetProgramInfoLog(
        program_id_,
        static_cast<GLsizei>(info_log.size()),
        nullptr,
        &info_log[0]
    );

    spdlog::error("Failed to link shader program ({})! Info log:\n{}", tag_, &info_log[0]);
    return false;
  }

  return true;
}

std::string ShaderBuilder::read_file_(const std::string &path) {
  std::ifstream t(path);
  std::string str;

  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  str.assign((std::istreambuf_iterator<char>(t)),
             std::istreambuf_iterator<char>());

  return str;
}

} // namespace horns
