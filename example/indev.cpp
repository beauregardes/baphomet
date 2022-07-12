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
#include "horns/shader.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

//// GLSL 1.2 to correspond with OpenGL 2.0
//#version 120
//
//// predefined variables
//uniform sampler2D tex0;
//
//float contour(in float d, in float w) {
//  // smoothstep(lower edge0, upper edge1, x)
//  return smoothstep(0.5 - w, 0.5 + w, d);
//}
//
//float samp(in vec2 uv, float w) {
//  return contour(texture2D(tex0, uv).a, w);
//}
//
//void main(void) {
//  // retrieve distance from texture
//  vec2 uv = gl_TexCoord[0].xy;
//  float dist = texture2D(tex0, uv).a;
//
//  // fwidth helps keep outlines a constant width irrespective of scaling
//  // GLSL's fwidth = abs(dFdx(uv)) + abs(dFdy(uv))
//  float width = fwidth(dist);
//  // Stefan Gustavson's fwidth
//  //float width = 0.7 * length(vec2(dFdx(dist), dFdy(dist)));
//
//  // basic version
//  //float alpha = smoothstep(0.5 - width, 0.5 + width, dist);
//
//  // supersampled version
//
//  float alpha = contour( dist, width );
//  //float alpha = aastep( 0.5, dist );
//
//  // ------- (comment this block out to get your original behavior)
//  // Supersample, 4 extra points
//  float dscale = 0.354; // half of 1/sqrt2; you can play with this
//  vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
//  vec4 box = vec4(uv-duv, uv+duv);
//
//  float asum = samp(box.xy, width)
//             + samp(box.zw, width)
//             + samp(box.xw, width)
//             + samp(box.zy, width);
//
//  // weighted average, with 4 extra points having 0.5 weight each,
//  // so 1 + 0.5*4 = 3 is the divisor
//  alpha = (alpha + 0.5 * asum) / 3.0;
//
//  // -------
//
//  gl_FragColor = vec4(gl_Color.rgb, alpha);
//}

namespace horns {

class Texture {
public:
  GLuint id{0};
  int width{0}, height{0};

  ~Texture() { del_id_(); }

  static std::unique_ptr<Texture> from_file(const std::filesystem::path &filename, bool retro);
  static std::unique_ptr<Texture> from_bytes(const void *bytes, int width, int height, bool retro);

  void bind(GLenum target=GL_TEXTURE_2D);

private:
  void gen_id_() { glGenTextures(1, &id); }
  void del_id_() { glDeleteTextures(1, &id); }

  Texture() { gen_id_(); }
};

std::unique_ptr<Texture> Texture::from_file(const std::filesystem::path &filename, bool retro) {
  int width, height;
  auto bytes = stbi_load(filename.string().c_str(), &width, &height, nullptr, 3);

  if (!bytes)
    return nullptr;
  else
    return from_bytes(bytes, width, height, retro);
}

std::unique_ptr<Texture> Texture::from_bytes(const void *bytes, int width, int height, bool retro) {
  auto tex = std::unique_ptr<Texture>(new Texture());
  tex->bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, retro ? GL_NEAREST : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, retro ? GL_NEAREST : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
//  glGenerateMipmap(GL_TEXTURE_2D);
  tex->width = width;
  tex->height = height;

  return tex;
}

void Texture::bind(GLenum target) {
  glBindTexture(target, id);
}

} // namespace horns

struct MSDFFont {
  msdfgen::FontHandle *fh{nullptr};
  std::unordered_map<msdf_atlas::unicode_t, msdf_atlas::GlyphGeometry> glyphs{};
  std::unique_ptr<horns::Texture> tex{nullptr};
};

const std::string lorem = R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque
ac magna vel velit luctus elementum id eu augue. Maecenas aliquet eros
ut maximus feugiat. Duis ultrices libero nec sapien varius, et posuere
dolor semper. Vivamus eget sapien erat. In magna diam, dignissim sed elit
convallis, pellentesque rutrum erat. Mauris facilisis et turpis sit amet
facilisis. Morbi sodales massa a sapien consequat pharetra. Aliquam suscipit
ipsum turpis, eget lobortis arcu convallis at.)";

class Indev : public goat::Application {
public:
  const std::filesystem::path RESOURCES = std::filesystem::path(__FILE__).remove_filename() / "resources";
  const std::filesystem::path FONT_PATH = RESOURCES / "Roboto-Regular.ttf";

  MSDFFont font;

  FT_Library ftlib;
  FT_Face face;

  std::unique_ptr<horns::Shader> shader{nullptr};

  glm::mat4 mvp{};

  float loadFontScale{4.0f};
  float maxCornerAngle{3.0f};
  float minimumScale{32.0f};
  float miterLimit{1.0f};
  float pxRangeUni{8.0f};
  float pxRange{8.0f};
  float fudge{1.0f};
  float gamma{1.0f};
  bool retroScaling{false};

  GLuint vao{0};
  GLuint vbo{0};

  int font_size = 16;
  int vertex_count = 0;

  void initialize() override {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FT_Init_FreeType(&ftlib);
    FT_New_Face(ftlib, FONT_PATH.string().c_str(), 0, &face);

    font = MSDFFont{};
    load_font(FONT_PATH);

    shader = horns::ShaderBuilder()
        .vert_from_file((RESOURCES / "msdf_shaders" / "opt1.vert").string())
        .frag_from_file((RESOURCES / "msdf_shaders" / "opt1.frag").string())
        .link();

    mvp = glm::ortho(0.0f, (float)window->w(), (float)window->h(), 0.0f, 0.0f, 1.0f);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 10, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(float) * 10, (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(float) * 10, (void *)(sizeof(float) * 6));
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
    msdf_atlas::utf8Decode(codepoints, lorem.c_str());

    float total_width = 0;
    float max_total_width = 0;
    for (const auto &cp : codepoints) {
      FT_Load_Char(face, cp, FT_LOAD_RENDER);
      if (cp == '\x0a') {
        if (total_width > max_total_width)
          max_total_width = total_width;
        total_width = 0;
        continue;
      }
      total_width += face->glyph->advance.x / 64.0f;
    }
    total_width = max_total_width;

    float x = (window->w() / 2.0) - (total_width / 2.0);
    float y = window->h() / 2.0 + 50;
    float ox = x;

    for (const auto &cp : codepoints) {
      auto error = FT_Load_Char(face, cp, FT_LOAD_RENDER);
      if (error)
        continue;

      if (cp == '\x0a') {
        x = ox;
        y += face->size->metrics.height / 64.0f;
        continue;
      }

      if (font.glyphs[cp].isWhitespace()) {
        x += face->glyph->advance.x / 64.0f;
        continue;
      }

      auto rect = font.glyphs[cp].getBoxRect();
      float tx = rect.x / (float) font.tex->width;
      float ty = rect.y / (float) font.tex->height;
      float tw = rect.w / (float) font.tex->width;
      float th = rect.h / (float) font.tex->height;

      auto size = glm::vec2{face->glyph->bitmap.width, face->glyph->bitmap.rows};
      auto bearing = glm::vec2{face->glyph->bitmap_left, face->glyph->bitmap_top};

      auto xpos = x + bearing.x;
      auto ypos = y + (size.y - bearing.y);

      float scale = pxRangeUni * (size.x / (float)(rect.w));

      vertices.insert(vertices.end(), {
          std::floor(xpos),          std::floor(ypos - size.y), 0.0f,   tx,      ty + th, scale,   1.0f, 1.0f, 1.0f, 1.0f,
          std::floor(xpos),          std::floor(ypos),          0.0f,   tx,      ty,      scale,   1.0f, 1.0f, 1.0f, 1.0f,
          std::floor(xpos + size.x), std::floor(ypos),          0.0f,   tx + tw, ty,      scale,   1.0f, 1.0f, 1.0f, 1.0f,
          std::floor(xpos),          std::floor(ypos - size.y), 0.0f,   tx,      ty + th, scale,   1.0f, 1.0f, 1.0f, 1.0f,
          std::floor(xpos + size.x), std::floor(ypos),          0.0f,   tx + tw, ty,      scale,   1.0f, 1.0f, 1.0f, 1.0f,
          std::floor(xpos + size.x), std::floor(ypos - size.y), 0.0f,   tx + tw, ty + th, scale,   1.0f, 1.0f, 1.0f, 1.0f,
      });
      vertex_count += 6;

      x += face->glyph->advance.x / 64.0f;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
  }

  void draw() override {
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (ImGui::Begin("Controls")) {
      ImGui::SliderInt("Text size", &font_size, 1, 96);
      ImGui::InputFloat("maxCornerAngle", &maxCornerAngle);
      ImGui::InputFloat("miterLimit", &miterLimit);
      ImGui::InputFloat("loadFontScale", &loadFontScale);
      ImGui::InputFloat("minimumScale", &minimumScale);
      ImGui::InputFloat("pxRange", &pxRange);
      ImGui::Checkbox("Retro", &retroScaling);
      if (ImGui::Button("Regenerate")) {
        load_font(FONT_PATH);
        pxRangeUni = pxRange;
      }
      ImGui::InputFloat("fudge", &fudge);
      ImGui::InputFloat("gamma", &gamma);
    }
    ImGui::End();

    regenerate_text(font_size);

    shader->use();
    shader->uniform_mat4f("MVP", mvp);
    shader->uniform_1f("fudge", fudge);
    shader->uniform_1f("gamma", gamma);
    font.tex->bind();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
  }

  void load_font(const std::filesystem::path &filename) {
    if (msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype()) {
      font.fh = msdfgen::loadFont(ft, filename.string().c_str());

      std::vector<msdf_atlas::GlyphGeometry> glyphs;
      msdf_atlas::FontGeometry fontGeometry(&glyphs);
      fontGeometry.loadCharset(font.fh, loadFontScale, msdf_atlas::Charset::ASCII);

      for (auto &glyph : glyphs)
        glyph.edgeColoring(&msdfgen::edgeColoringByDistance, maxCornerAngle, 0);

      msdf_atlas::TightAtlasPacker packer;
      packer.setDimensionsConstraint(msdf_atlas::TightAtlasPacker::DimensionsConstraint::SQUARE);
      packer.setMinimumScale(minimumScale);
      packer.setPixelRange(pxRange);
      packer.setMiterLimit(miterLimit);
      packer.pack(glyphs.data(), glyphs.size());

      int width = 0, height = 0;
      packer.getDimensions(width, height);
      fmt::print("Atlas is {}x{}\n", width, height);

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
        font.glyphs[glyph.getCodepoint()] = glyph;

      auto bitmap = static_cast<msdfgen::BitmapConstRef<msdf_atlas::byte, 3>>(generator.atlasStorage());
      font.tex = horns::Texture::from_bytes(bitmap.pixels, bitmap.width, bitmap.height, retroScaling);

      msdfgen::deinitializeFreetype(ft);
    }
  }
};

int main(int, char *[]) {
  goat::Engine({
      .backend_version = {3, 3},
      .log_level = spdlog::level::debug
  }).open<Indev>({
      .title = "Indev",
      .size = {1280, 960},
      .monitor = 1,
      .flags = goat::WFlags::centered
  });
}
