#ifndef GL_OVAL_BATCH_HPP
#define GL_OVAL_BATCH_HPP

#include "gl/batching/batch.hpp"

namespace gl {

class OvalBatch : public Batch {
public:
    OvalBatch(GladGLContext *ctx);
    ~OvalBatch() = default;

    void add(
        float x, float y,
        float x_radius, float y_radius,
        float z,
        float r, float g, float b, float a,
        float cx, float cy, float angle
    );

    void draw_opaque(float z_max, glm::mat4 projection) override;
    void draw_alpha(float z_max, glm::mat4 projection) override;

private:
    void add_opaque_(
        float x, float y,
        float x_radius, float y_radius,
        float z,
        float r, float g, float b, float a,
        float cx, float cy, float angle
    );

    void add_opaque_recurse_(
        float x, float y,
        float x_radius, float y_radius,
        float z,
        float r, float g, float b, float a,
        float cx, float cy, float angle,
        float x0, float y0, float a0,
        float x1, float y1, float a1
    );

    void add_alpha_(
        float x, float y,
        float x_radius, float y_radius,
        float z,
        float r, float g, float b, float a,
        float cx, float cy, float angle
    );

    void add_alpha_recurse_(
        float x, float y,
        float x_radius, float y_radius,
        float z,
        float r, float g, float b, float a,
        float cx, float cy, float angle,
        float x0, float y0, float a0,
        float x1, float y1, float a1
    );
};

} // namespace gl

#endif //GL_OVAL_BATCH_HPP