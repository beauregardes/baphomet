#pragma once

#include "baphomet/gfx/gl/texture_unit.hpp"

#include <filesystem>
#include <unordered_map>

namespace baphomet {

class ResourceLoader {
public:
  ResourceLoader() = default;
  ~ResourceLoader() = default;

  void load_texture_unit(const std::string &name, const std::string &path, bool retro = false);
  void load_texture_unit(const std::string &name, const std::filesystem::path &path, bool retro = false);

  const std::unique_ptr<gl::TextureUnit> &get_texture_unit(const std::string &name);

  static std::string resolve_resource_path(const std::string &path);

private:
  std::unordered_map<std::string, std::unique_ptr<gl::TextureUnit>> texture_units_{};

  static const std::filesystem::path &resource_path_();
};

} // namespace baphomet
