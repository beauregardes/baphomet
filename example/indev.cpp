#include "goat/goat.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "msdf-atlas-gen/msdf-atlas-gen.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <filesystem>
#include <unordered_map>
#include <vector>
#include "ft2build.h"
#include FT_FREETYPE_H

namespace horns {

class Texture {
public:
  GLuint id{0};
  int width{0}, height{0};

  ~Texture() { del_id_(); }

  static std::unique_ptr<Texture> from_file(const std::filesystem::path &filename);
  static std::unique_ptr<Texture> from_bytes(const void *bytes, int width, int height);

  void bind(GLenum target=GL_TEXTURE_2D);

private:
  void gen_id_() { glGenTextures(1, &id); }
  void del_id_() { glDeleteTextures(1, &id); }

  Texture() { gen_id_(); }
};

std::unique_ptr<Texture> Texture::from_file(const std::filesystem::path &filename) {
  int width, height;
  auto bytes = stbi_load(filename.string().c_str(), &width, &height, nullptr, 3);

  if (!bytes)
    return nullptr;
  else
    return from_bytes(bytes, width, height);
}

std::unique_ptr<Texture> Texture::from_bytes(const void *bytes, int width, int height) {
  auto tex = std::unique_ptr<Texture>(new Texture());
  tex->bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
  tex->width = width;
  tex->height = height;

  return tex;
}

void Texture::bind(GLenum target) {
  glBindTexture(target, id);
}

} // namespace sin

struct MSDFFont {
  msdfgen::FontHandle *fh{nullptr};
  std::unordered_map<msdf_atlas::unicode_t, msdf_atlas::GlyphGeometry> glyphs{};
  std::unique_ptr<horns::Texture> tex{nullptr};
};
MSDFFont load_font(const std::filesystem::path &filename);

const std::string vsh_src = R"glsl(
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec4 vertexColor;

out vec2 texCoords;
out vec4 fragColor;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
    texCoords = textureCoordinates;
    fragColor = vertexColor;
}
)glsl";

//const std::string fsh_src = R"glsl(
//#version 330 core
//
//in vec2 texCoords;
//in vec4 fragColor;
//
//out vec4 color;
//
//uniform sampler2D msdf;
//
//float median(float r, float g, float b) {
//    return max(min(r, g), min(max(r, g), b));
//}
//
//void main() {
//    vec3 sample = texture(msdf, texCoords).rgb;
//    ivec2 sz = textureSize(msdf, 0);
//    float dx = dFdx( texCoords.x ) * sz.x;
//    float dy = dFdy( texCoords.y ) * sz.y;
//    float toPixels = 8.0 * inversesqrt( dx * dx + dy * dy );
//    float sigDist = median( sample.r, sample.g, sample.b ) - 0.5;
//    float opacity = clamp( sigDist * toPixels + 0.5, 0.0, 1.0 );
//    color = vec4(fragColor.rgb, opacity);
//}
//)glsl";

const std::string fsh_src = R"glsl(
#version 330 core

in vec2 texCoords;
in vec4 fragColor;

out vec4 color;

uniform sampler2D msdf;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec3 sample = texture(msdf, texCoords).rgb;
    ivec2 sz = textureSize(msdf, 0).xy;
    float dx = dFdx(texCoords.x) * sz.x;
    float dy = dFdy(texCoords.y) * sz.y;
    float toPixels = 8.0 * inversesqrt(dx * dx + dy * dy);
    float sigDist = median(sample.r, sample.g, sample.b);
    float w = fwidth(sigDist);
    float opacity = smoothstep(0.5 - w, 0.5 + w, sigDist);
    color = mix(vec4(0.0, 0.0, 0.0, 0.0), fragColor.rgba, opacity);
}
)glsl";

bool check_compile_(GLuint shader_id, GLenum type) {
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

    spdlog::error("Failed to compile {}! Info log:\n{}", type_str, &info_log[0]);
    return false;
  }

  return true;
}

bool check_link_(GLuint program_id) {
  static auto info_log = std::vector<char>();
  static int success;

  glGetProgramiv(program_id, GL_LINK_STATUS, &success);
  if (!success) {
    info_log.clear();
    info_log.resize(512);
    glGetProgramInfoLog(
        program_id,
        static_cast<GLsizei>(info_log.size()),
        nullptr,
        &info_log[0]
    );

    spdlog::error("Failed to link shader program! Info log:\n{}", &info_log[0]);
    return false;
  }

  return true;
}

class Indev : public goat::Application {
public:
  const std::filesystem::path RESOURCES = std::filesystem::path(__FILE__).remove_filename() / "resources";

  MSDFFont font;

  FT_Library ftlib;
  FT_Face face;

  GLuint shprogram{0};
  GLint projection_loc{0};
  glm::mat4 projection{};

  GLuint vao{0};
  GLuint vbo{0};

  int font_size = 16;
  int vertex_count = 0;

  void initialize() override {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FT_Init_FreeType(&ftlib);
    FT_New_Face(ftlib, (RESOURCES / "CascadiaCode.ttf").string().c_str(), 0, &face);
    FT_Set_Char_Size(face, 0, 16 * 64, 72, 0);

    font = load_font(RESOURCES / "CascadiaCode.ttf");

    auto vsh = glCreateShader(GL_VERTEX_SHADER);
    auto vsh_src_p = vsh_src.c_str();
    glShaderSource(vsh, 1, &vsh_src_p, nullptr);
    glCompileShader(vsh);
    check_compile_(vsh, GL_VERTEX_SHADER);

    auto fsh = glCreateShader(GL_FRAGMENT_SHADER);
    auto fsh_src_p = fsh_src.c_str();
    glShaderSource(fsh, 1, &fsh_src_p, nullptr);
    glCompileShader(fsh);
    check_compile_(fsh, GL_FRAGMENT_SHADER);

    shprogram = glCreateProgram();
    glAttachShader(shprogram, vsh);
    glAttachShader(shprogram, fsh);
    glLinkProgram(shprogram);
    check_link_(shprogram);

    glDeleteShader(vsh);
    glDeleteShader(fsh);

    projection_loc = glGetUniformLocation(shprogram, "MVP");
    projection = glm::ortho(0.0f, (float)window->w(), (float)window->h(), 0.0f, 0.0f, 1.0f);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 9, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(float) * 9, (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(float) * 9, (void *)(sizeof(float) * 5));
    glEnableVertexAttribArray(2);
  }

  void update(double dt) override {
    if (input->pressed("escape"))
      window->set_should_close(true);
  }

  void regenerate_text(int font_size) {
    FT_Set_Char_Size(face, 0, font_size * 64, 96, 0);

    std::vector<float> vertices{};
    vertex_count = 0;

    std::vector<msdf_atlas::unicode_t> codepoints;
    msdf_atlas::utf8Decode(codepoints, "Hello, world!");
    float x = input->mouse.x;
    float y = input->mouse.y - 10;
    for (const auto &cp : codepoints) {
      FT_Load_Char(face, cp, FT_LOAD_COMPUTE_METRICS);
      auto metrics = face->glyph->metrics;

      if (cp != ' ') {
        auto rect = font.glyphs[cp].getBoxRect();
        float tx = rect.x / (float) font.tex->width;
        float ty = rect.y / (float) font.tex->height;
        float tw = rect.w / (float) font.tex->width;
        float th = rect.h / (float) font.tex->height;

        vertices.insert(vertices.end(), {
            x, y - (metrics.horiBearingY >> 6), 0.0f, tx, ty + th, 1.0f, 1.0f, 1.0f, 1.0f,
            x + (metrics.width >> 6), y - (metrics.horiBearingY >> 6), 0.0f, tx + tw, ty + th, 1.0f, 1.0f, 1.0f, 1.0f,
            x + (metrics.width >> 6), y - (metrics.horiBearingY >> 6) + (metrics.height >> 6), 0.0f, tx + tw, ty, 1.0f,
            1.0f, 1.0f, 1.0f,
            x, y - (metrics.horiBearingY >> 6), 0.0f, tx, ty + th, 1.0f, 1.0f, 1.0f, 1.0f,
            x + (metrics.width >> 6), y - (metrics.horiBearingY >> 6) + (metrics.height >> 6), 0.0f, tx + tw, ty, 1.0f,
            1.0f, 1.0f, 1.0f,
            x, y - (metrics.horiBearingY >> 6) + (metrics.height >> 6), 0.0f, tx, ty, 1.0f, 1.0f, 1.0f, 1.0f,
        });
        vertex_count += 6;
      }

      x += metrics.horiAdvance >> 6;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
  }

  void draw() override {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (ImGui::Begin("Controls")) {
      ImGui::SliderInt("Font size", &font_size, 8, 72);
    }
    ImGui::End();

    regenerate_text(font_size);

    glUseProgram(shprogram);
    glUniformMatrix4fv(projection_loc, 1, false, glm::value_ptr(projection));
    font.tex->bind();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
  }
};

MSDFFont load_font(const std::filesystem::path &filename) {
  MSDFFont ret{};

  if (msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype()) {
    ret.fh = msdfgen::loadFont(ft, filename.string().c_str());

    std::vector<msdf_atlas::GlyphGeometry> glyphs;
    msdf_atlas::FontGeometry fontGeometry(&glyphs);
    fontGeometry.loadCharset(ret.fh, 2.0, msdf_atlas::Charset::ASCII);

    const double maxCornerAngle = 3.0;
    for (auto &glyph : glyphs)
      glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);

    msdf_atlas::TightAtlasPacker packer;
    packer.setDimensionsConstraint(msdf_atlas::TightAtlasPacker::DimensionsConstraint::POWER_OF_TWO_RECTANGLE);
    packer.setMinimumScale(96.0);
    packer.setPixelRange(12.0);
    packer.setMiterLimit(1.0);
    packer.pack(glyphs.data(), glyphs.size());

    int width = 0, height = 0;
    packer.getDimensions(width, height);

    msdf_atlas::ImmediateAtlasGenerator<
        float,
        3,
        msdf_atlas::msdfGenerator,
        msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>
    > generator(width, height);

    msdf_atlas::GeneratorAttributes attributes;
    attributes.config.errorCorrection.mode = msdfgen::ErrorCorrectionConfig::EDGE_PRIORITY;
    attributes.config.errorCorrection.distanceCheckMode = msdfgen::ErrorCorrectionConfig::ALWAYS_CHECK_DISTANCE;
    attributes.scanlinePass = true;
    generator.setAttributes(attributes);
    generator.setThreadCount(4);
    generator.generate(glyphs.data(), glyphs.size());

    for (const auto &glyph : glyphs)
      ret.glyphs[glyph.getCodepoint()] = glyph;

    auto bitmap = static_cast<msdfgen::BitmapConstRef<msdf_atlas::byte, 3>>(generator.atlasStorage());
    ret.tex = horns::Texture::from_bytes(bitmap.pixels, bitmap.width, bitmap.height);

    msdfgen::deinitializeFreetype(ft);
  }

  return ret;
}

int main(int, char *[]) {
  goat::Engine({
      .backend_version = {3, 3},
      .log_level = spdlog::level::debug
  }).open<Indev>({
      .title = "Indev",
      .size = {800, 600},
      .monitor = 0,
      .flags = goat::WFlags::centered
  });
}
