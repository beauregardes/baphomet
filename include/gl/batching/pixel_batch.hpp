#ifndef GL_PIXEL_BATCH_HPP
#define GL_PIXEL_BATCH_HPP

#include "gl/batching/batch.hpp"

namespace gl {

class PixelBatch : public Batch {
public:
    PixelBatch(GladGLContext *ctx);
    ~PixelBatch() = default;

    void add(
        float x, float y,
        float z,
        float r, float g, float b, float a
    );

    void draw(float z_max, glm::mat4 projection) override;
};

} // namespace gl

#endif //GL_PIXEL_BATCH_HPP
