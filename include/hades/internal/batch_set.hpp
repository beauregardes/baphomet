#ifndef HADES_BATCH_SET_HPP
#define HADES_BATCH_SET_HPP

#include "gl/batching/line_batch.hpp"
#include "gl/batching/oval_batch.hpp"
#include "gl/batching/pixel_batch.hpp"
#include "gl/batching/rect_batch.hpp"
#include "gl/batching/texture_batch.hpp"
#include "gl/batching/tri_batch.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace hades {

class BatchSet {
public:
    float z_level{1.0f};

    std::unique_ptr <gl::PixelBatch> pixels{nullptr};
    std::unique_ptr <gl::LineBatch> lines{nullptr};
    std::unique_ptr <gl::TriBatch> tris{nullptr};
    std::unique_ptr <gl::RectBatch> rects{nullptr};
    std::unique_ptr <gl::OvalBatch> ovals{nullptr};

    std::unordered_map <std::string, std::unique_ptr<gl::TextureBatch>> textures{};

    void clear();

    std::size_t pixel_vertex_count_opaque() const;
    std::size_t line_vertex_count_opaque() const;
    std::size_t tri_vertex_count_opaque() const;
    std::size_t rect_vertex_count_opaque() const;
    std::size_t oval_vertex_count_opaque() const;
    std::size_t texture_vertex_count_opaque() const;

    std::size_t pixel_vertex_count_alpha() const;
    std::size_t line_vertex_count_alpha() const;
    std::size_t tri_vertex_count_alpha() const;
    std::size_t rect_vertex_count_alpha() const;
    std::size_t oval_vertex_count_alpha() const;
    std::size_t texture_vertex_count_alpha() const;

    void draw_opaque(glm::mat4 projection);
    void draw_alpha(glm::mat4 projection);
};

} // namespace hades

#endif //HADES_BATCH_SET_HPP
