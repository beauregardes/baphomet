#pragma once

#include "baphomet/gfx/gl/texture_unit.hpp"

#include <filesystem>
#include <unordered_map>

namespace baphomet {

const static std::filesystem::path RESOURCE_PATH =
    std::filesystem::path(__FILE__)
        .parent_path()
        .parent_path()
        .parent_path()
        .parent_path()
        .parent_path()
    / "resources";

class ResourceLoader {
public:
  ResourceLoader() = default;
  ~ResourceLoader() = default;

  void load_texture_unit(const std::string &name, const std::string &path, bool retro = false);
  void load_texture_unit(const std::string &name, const std::filesystem::path &path, bool retro = false);

  const std::unique_ptr<gl::TextureUnit> &get_texture_unit(const std::string &name);

private:
  std::unordered_map<std::string, std::unique_ptr<gl::TextureUnit>> texture_units_{};
};

} // namespace baphomet
