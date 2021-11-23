#include "gl/texture_unit.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <spdlog/spdlog.h>

namespace gl {

TextureUnit::TextureUnit(GladGLContext *ctx, const std::string &path, bool retro) : ctx_(ctx) {
    stbi_set_flip_vertically_on_load(false);

    gen_id_();
    bind();

    ctx_->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    ctx_->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    ctx_->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, retro ? GL_NEAREST : GL_LINEAR);
    ctx_->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, retro ? GL_NEAREST : GL_LINEAR);

    int w, h, comp;
    auto bytes = stbi_load(path.c_str(), &w, &h, &comp, 0);

    if (bytes) {
        width_ = w;
        height_ = h;

        GLenum format = GL_NONE;
        if (comp == 3)
            format = GL_RGB;
        else if (comp == 4)
            format = GL_RGBA;
        else
            spdlog::error("Can't handle images with comp '{}', only 3 or 4 channels supported", comp);

        if (format != GL_NONE) {
            ctx_->TexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, bytes);
            ctx_->GenerateMipmap(GL_TEXTURE_2D);

            spdlog::debug("Loaded texture '{}' ({}x{})", path, width_, height_);
        }

        if (comp == 4) {
            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                    int alpha_ch = (y * w * 4) + (x * 4) + 3;
                    if (bytes[alpha_ch] < 255)
                        fully_opaque_ = false;
                }
                if (!fully_opaque_)
                    break;
            }
        }

    } else
        spdlog::error("Failed to load texture '{}': {}", path, stbi_failure_reason());

    stbi_image_free(bytes);
    unbind();
}

TextureUnit::TextureUnit(GladGLContext *ctx, const std::filesystem::path &path, bool retro)
    : TextureUnit(ctx, path.string(), retro) {}

TextureUnit::~TextureUnit() {
    del_id_();
    ctx_ = nullptr;
}

TextureUnit::TextureUnit(TextureUnit &&other) {
    ctx_ = other.ctx_;
    id_ = other.id_;
    width_ = other.width_;
    height_ = other.height_;

    other.ctx_ = nullptr;
    other.id_ = 0;
    other.width_ = 0;
    other.height_ = 0;
}

TextureUnit &TextureUnit::operator=(TextureUnit &&other) {
    if (this != &other) {
        del_id_();

        ctx_ = other.ctx_;
        id_ = other.id_;
        width_ = other.width_;
        height_ = other.height_;

        other.ctx_ = nullptr;
        other.id_ = 0;
        other.width_ = 0;
        other.height_ = 0;
    }
    return *this;
}

GLuint TextureUnit::id() const {
    return id_;
}

void TextureUnit::bind(int unit) {
    ctx_->ActiveTexture(GL_TEXTURE0 + unit);
    ctx_->BindTexture(GL_TEXTURE_2D, id_);
}

void TextureUnit::unbind() {
    ctx_->BindTexture(GL_TEXTURE_2D, 0);
}

GLuint TextureUnit::width() const {
    return width_;
}

GLuint TextureUnit::height() const {
    return height_;
}

bool TextureUnit::fully_opaque() const {
    return fully_opaque_;
}

void TextureUnit::gen_id_() {
    ctx_->GenTextures(1, &id_);
    spdlog::debug("Generated texture ({})", id_);
}

void TextureUnit::del_id_() {
    if (ctx_ && id_ != 0) {
        ctx_->DeleteTextures(1, &id_);
        spdlog::debug("Deleted texture ({})", id_);
    }
}


} // namespace gl