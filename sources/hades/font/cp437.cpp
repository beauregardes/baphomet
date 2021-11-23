#include "hades/font/cp437.hpp"

namespace hades {

CP437::CP437(
    const std::unique_ptr<gl::TextureBatch> &batch,
    GLuint width, GLuint height,
    GLuint char_w, GLuint char_h,
    float &z_level
) : Texture(batch, width, height, z_level), char_w_(char_w), char_h_(char_h) {}

GLuint CP437::char_w() const {
    return char_w_;
}

GLuint CP437::char_h() const {
    return char_h_;
}

void CP437::render_(float x, float y, float scale, const hades::RGB &color, fmt::string_view format, fmt::format_args args) {
    std::string text = fmt::vformat(format, args);

    float curr_x = x, curr_y = y;
    for (const auto &c : text) {
        int idx = static_cast<int>(c);

        if (idx == 10) {
            curr_y += scale * char_h_;
            curr_x = x;
            continue;

        } else if (idx == 32) {
            curr_x += scale * char_w_;
            continue;

        } else if (idx >= 33 && idx <= 126) {
            float tx = char_w_ * (idx % 16);
            float ty = char_h_ * ((idx / 16) - 2);

            draw(
                curr_x, curr_y,
                scale * char_w_, scale * char_h_,
                tx, ty,
                char_w_, char_h_,
                color
            );

            curr_x += scale * char_w_;
        }
    }
}

} // namespace hades