#include "hades/internal/resource_loader.hpp"

namespace hades {

ResourceLoader::ResourceLoader(GladGLContext *ctx)
    : ctx_(ctx) {}

void ResourceLoader::load_texture_unit(const std::string &name, const std::string &path, bool retro) {
    texture_units_[name] = std::make_unique<gl::TextureUnit>(ctx_, path, retro);
}

void ResourceLoader::load_texture_unit(const std::string &name, const std::filesystem::path &path, bool retro) {
    texture_units_[name] = std::make_unique<gl::TextureUnit>(ctx_, path, retro);
}

const std::unique_ptr<gl::TextureUnit> &ResourceLoader::get_texture_unit(const std::string &name) {
    return texture_units_[name];
}

} // namespace hades