#include "gl/buffer_base.hpp"

#include "spdlog/spdlog.h"

namespace gl {

BufferBase::BufferBase(GladGLContext *ctx)
    : ctx_(ctx) {}

BufferBase::~BufferBase() {
    del_id_();
    ctx_ = nullptr;  // This isn't ours, don't delete it
}

BufferBase::BufferBase(BufferBase &&other) noexcept {
    id = other.id;
    ctx_ = other.ctx_;

    other.id = 0;
    other.ctx_ = nullptr;
}

BufferBase &BufferBase::operator=(BufferBase &&other) noexcept {
    if (this != &other) {
        del_id_();

        id = other.id;
        ctx_ = other.ctx_;

        other.id = 0;
        other.ctx_ = nullptr;
    }
    return *this;
}

void BufferBase::bind(BufTarget target) const {
    ctx_->BindBuffer(unwrap(target), id);
}

void BufferBase::unbind(BufTarget target) const {
    ctx_->BindBuffer(unwrap(target), 0);
}

void BufferBase::gen_id_() {
    ctx_->GenBuffers(1, &id);
    spdlog::debug("Generated buffer ({})", id);
}

void BufferBase::del_id_() {
    if (id != 0) {
        ctx_->DeleteBuffers(1, &id);
        spdlog::debug("Deleted buffer ({})", id);
    }
}

} // namespace gl