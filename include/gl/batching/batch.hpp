#ifndef GL_BATCH_HPP
#define GL_BATCH_HPP

#include "gl/shader.hpp"
#include "gl/vec_buffer.hpp"
#include "gl/vertex_array.hpp"
#include "gl/batching/batch.hpp"

#include "glm/glm.hpp"

#include <memory>

namespace gl {

class Batch {
public:
    Batch(GladGLContext *ctx);

    virtual void clear();

    virtual void draw(float z_max, glm::mat4 projection) = 0;

protected:
    GladGLContext *ctx_{nullptr};

    std::unique_ptr<Shader> shader_{nullptr};
    std::unique_ptr<VertexArray> vao_{nullptr};
    std::unique_ptr<VecBuffer<float>> vertices_{nullptr};
};

} // namespace gl

#endif //GL_BATCH_HPP