#ifndef HADES_TEXTURE_HPP
#define HADES_TEXTURE_HPP

#include "gl/batching/texture_batch.hpp"
#include "hades/color.hpp"

namespace hades {

class Texture {
public:
    Texture(const std::unique_ptr<gl::TextureBatch> &batch, GLuint width, GLuint height, float &z_level);

    GLuint width() const;
    GLuint height() const;

    void draw(
        float x, float y, float w, float h,
        float tx, float ty, float tw, float th,
        float cx, float cy, float angle,
        const hades::RGB &color = hades::rgb(0xffffff)
    );

    void draw(
        float x, float y, float w, float h,
        float tx, float ty, float tw, float th,
        const hades::RGB &color = hades::rgb(0xffffff)
    );

    void draw(
        float x, float y, float w, float h,
        float cx, float cy, float angle,
        const hades::RGB &color = hades::rgb(0xffffff)
    );

    void draw(
        float x, float y, float w, float h,
        const hades::RGB &color = hades::rgb(0xffffff)
    );

    void draw(
        float x, float y,
        float cx, float cy, float angle,
        const hades::RGB &color = hades::rgb(0xffffff)
    );

    void draw(
        float x, float y,
        const hades::RGB &color = hades::rgb(0xffffff)
    );

private:
    const std::unique_ptr<gl::TextureBatch> &batch_;
    float &z_level_;

    GLuint width_{0}, height_{0};
};

} // namespace hades

#endif //HADES_TEXTURE_HPP
