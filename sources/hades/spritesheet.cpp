#include "hades/spritesheet.hpp"

namespace hades {

Spritesheet::Spritesheet(
  const std::unique_ptr<BatchSet> &bs,
  const std::string &name,
  std::unordered_map<std::string, glm::vec4> mappings,
  float tile_w, float tile_h
) : bs_(bs), name_(name), mappings_(mappings), tile_w_(tile_w), tile_h_(tile_h) {}

float Spritesheet::tile_w() const {
  return tile_w_;
}

float Spritesheet::tile_h() const {
  return tile_h_;
}

void Spritesheet::draw(
  const std::string &name, 
  float x, float y, float w, float h,
  float cx, float cy, float angle,
  const hades::RGB &color
) {
  bs_->add_texture(
    name_,
    x, y, w, h,
    mappings_[name].x, mappings_[name].y, mappings_[name].z, mappings_[name].w,
    cx, cy, angle,
    color
  );
}

void Spritesheet::draw(
  const std::string &name, 
  float x, float y, float w, float h,
  const hades::RGB &color
) {
  bs_->add_texture(
    name_,
    x, y, w, h,
    mappings_[name].x, mappings_[name].y, mappings_[name].z, mappings_[name].w,
    0.0f, 0.0f, 0.0f,
    color
  );
}

void Spritesheet::draw(
  const std::string &name, 
  float x, float y, 
  float cx, float cy, float angle,
  const hades::RGB &color
) {
  bs_->add_texture(
    name_,
    x, y, mappings_[name].z, mappings_[name].w,
    mappings_[name].x, mappings_[name].y, mappings_[name].z, mappings_[name].w,
    cx, cy, angle,
    color
  );
}

void Spritesheet::draw(
  const std::string &name, 
  float x, float y, 
  const hades::RGB &color
) {
  bs_->add_texture(
    name_,
    x, y, mappings_[name].z, mappings_[name].w,
    mappings_[name].x, mappings_[name].y, mappings_[name].z, mappings_[name].w,
    0.0f, 0.0f, 0.0f,
    color
  );
}

SpritesheetBuilder::SpritesheetBuilder(const std::unique_ptr<BatchSet> &bs, const std::string &name)
  : bs_(bs), name_(name) {}

SpritesheetBuilder &SpritesheetBuilder::load_ini(const std::string &path) {
  // TODO: NYI
  return *this;
}

SpritesheetBuilder &SpritesheetBuilder::set_tiled(float w, float h) {
  tiled_ = true;
  tile_w_ = w;
  tile_h_ = h;
  return *this;
}

SpritesheetBuilder &SpritesheetBuilder::add_sprite(
  const std::string &name, 
  float x, float y
) {
  mappings_[name] = {
    x * (tiled_ ? tile_w_ : 1),
    y * (tiled_ ? tile_h_ : 1),
    tiled_ ? tile_w_ : 0,
    tiled_ ? tile_h_ : 0
  };
  return *this;
}

SpritesheetBuilder &SpritesheetBuilder::add_sprite(
  const std::string &name, 
  float x, float y, float w, float h
) {
  mappings_[name] = {
    x * (tiled_ ? tile_w_ : 1),
    y * (tiled_ ? tile_h_ : 1),
    w * (tiled_ ? tile_w_ : 1),
    h * (tiled_ ? tile_h_ : 1)
  };
  return *this;
}

SpritesheetBuilder &SpritesheetBuilder::add_sprite(
  const std::string &name, 
  float x, float y, 
  float x_offset, float y_offset,
  float w, float h
) {
  mappings_[name] = {
    x_offset + x * (tiled_ ? tile_w_ : 1),
    y_offset + y * (tiled_ ? tile_h_ : 1),
    w * (tiled_ ? tile_w_ : 1),
    h * (tiled_ ? tile_h_ : 1)
  };
  return *this;
}

std::unique_ptr<Spritesheet> SpritesheetBuilder::build() {
  return std::make_unique<Spritesheet>(bs_, name_, mappings_, tile_w_, tile_h_);
}

} // namespace hades