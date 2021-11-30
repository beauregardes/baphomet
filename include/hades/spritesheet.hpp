#ifndef HADES_SPRITESHEET_HPP
#define HADES_SPRITESHEET_HPP

#include "hades/color.hpp"
#include "hades/texture.hpp"
#include "hades/internal/batch_set.hpp"

#include <string>
#include <unordered_map>

namespace hades {

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
    const hades::RGB &color = hades::rgb(0xffffff)
  );

  void draw(
    const std::string &name, 
    float x, float y, float w, float h,
    const hades::RGB &color = hades::rgb(0xffffff)
  );

  void draw(
    const std::string &name, 
    float x, float y, 
    float cx, float cy, float angle,
    const hades::RGB &color = hades::rgb(0xffffff)
  );

  void draw(
    const std::string &name, 
    float x, float y, 
    const hades::RGB &color = hades::rgb(0xffffff)
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

} // namespace hades

#endif //HADES_SPRITESHEET_HPP
