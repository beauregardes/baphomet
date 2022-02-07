#pragma once

#include "baphomet/gfx/internal/batch_set.hpp"
#include "baphomet/gfx/color.hpp"
#include "baphomet/gfx/texture.hpp"

#include <string>
#include <unordered_map>

namespace baphomet {

class Spritesheet {
public:
  Spritesheet(
      const std::unique_ptr<BatchSet> &bs,
      const std::string &name,
      std::unordered_map<std::string, glm::vec4> mappings,
      float tile_w, float tile_h
  );

  float tile_w() const;
  float tile_h() const;

  void draw(
      const std::string &name,
      float x, float y, float w, float h,
      float cx, float cy, float angle,
      const baphomet::RGB &color = rgb(0xffffff)
  );

  void draw(
      const std::string &name,
      float x, float y, float w, float h,
      const baphomet::RGB &color = rgb(0xffffff)
  );

  void draw(
      const std::string &name,
      float x, float y,
      float cx, float cy, float angle,
      const baphomet::RGB &color = rgb(0xffffff)
  );

  void draw(
      const std::string &name,
      float x, float y,
      const baphomet::RGB &color = rgb(0xffffff)
  );

private:
  const std::unique_ptr<BatchSet> &bs_;
  std::string name_{};

  std::unordered_map<std::string, glm::vec4> mappings_{};

  float tile_w_{0}, tile_h_{0};
};

class SpritesheetBuilder {
public:
  SpritesheetBuilder(const std::unique_ptr<BatchSet> &bs, const std::string &name);

  SpritesheetBuilder &load_ini(const std::string &path);

  SpritesheetBuilder &set_tiled(float w, float h);

  SpritesheetBuilder &add_sprite(
      const std::string &name,
      float x, float y
  );

  SpritesheetBuilder &add_sprite(
      const std::string &name,
      float x, float y, float w, float h
  );

  SpritesheetBuilder &add_sprite(
      const std::string &name,
      float x, float y,
      float x_offset, float y_offset,
      float w, float h
  );

  std::unique_ptr<Spritesheet> build();

private:
  const std::unique_ptr<BatchSet> &bs_;
  std::string name_{};

  std::unordered_map<std::string, glm::vec4> mappings_{};

  bool tiled_{false};
  float tile_w_{0}, tile_h_{0};
};

} // namespace baphomet
