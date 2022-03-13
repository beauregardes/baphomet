#include "baphomet/app/internal/resource_loader.hpp"

namespace baphomet {

std::string ResourceLoader::resolve_resource_path(const std::string &path) {
  return (resource_path_() / path).string();
}

void ResourceLoader::load_texture_unit(const std::string &name, const std::string &path, bool retro) {
  texture_units_[name] = std::make_shared<gl::TextureUnit>(path, retro);
}

void ResourceLoader::load_texture_unit(const std::string &name, const std::filesystem::path &path, bool retro) {
  texture_units_[name] = std::make_shared<gl::TextureUnit>(path, retro);
}

const std::shared_ptr<gl::TextureUnit> &ResourceLoader::get_texture_unit(const std::string &name) {
  return texture_units_[name];
}

const std::filesystem::path &ResourceLoader::resource_path_() {
  const static std::filesystem::path RESOURCE_PATH =
      std::filesystem::path(__FILE__)
          .parent_path()
          .parent_path()
          .parent_path()
          .parent_path()
          .parent_path()
      / "resources";

  return RESOURCE_PATH;
}

} // namespace baphomet
