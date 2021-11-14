#ifndef HADES_GL_STATICBUFFER_HPP
#define HADES_GL_STATICBUFFER_HPP

#include "hades/gl/buffer_base.hpp"
#include "spdlog/spdlog.h"

namespace gl {

template<typename T>
class StaticBuffer : public BufferBase {
public:
    StaticBuffer(GladGLContext *ctx, GLsizeiptr size, BufTarget target, BufUsage usage)
        : BufferBase(ctx) {

        target_ = target;
        usage_ = usage;

        gen_id_();
        resize(size);
    }

    StaticBuffer(GladGLContext *ctx, const std::vector<T> &data, BufTarget target, BufUsage usage)
        : BufferBase(ctx) {

        target_ = target;
        usage_ = usage;

        gen_id_();
        write(data);
    }

    ~StaticBuffer() override = default;

    // Copy constructors don't make sense for OpenGL objects
    StaticBuffer(const StaticBuffer &) = delete;
    StaticBuffer &operator=(const StaticBuffer &) = delete;

    StaticBuffer(StaticBuffer &&other) noexcept: BufferBase(std::move(other)) {
        size_ = other.size_;
        target_ = other.target_;
        usage_ = other.usage_;

        other.size_ = 0;
        other.target_ = BufTarget::none;
        other.usage_ = BufUsage::none;
    }

    StaticBuffer &operator=(StaticBuffer &&other) noexcept {
        if (this != &other) {
            BufferBase::operator=(std::move(other));

            size_ = other.size_;
            target_ = other.target_;
            usage_ = other.usage_;

            other.size_ = 0;
            other.target_ = BufTarget::none;
            other.usage_ = BufUsage::none;
        }
        return *this;
    }

    GLsizeiptr size() {
        return size_;
    }

    void resize(GLsizeiptr new_size, BufTarget target, BufUsage usage) {
        bind(target);
        ctx_->BufferData(unwrap(target), sizeof(T) * new_size, nullptr, unwrap(usage));
        unbind(target);

        size_ = new_size;
        target_ = target;
        usage_ = usage;
    }

    void resize(GLsizeiptr size) {
        resize(size, target_, usage_);
    }

    void write(const std::vector<T> &new_data, BufTarget target, BufUsage usage) {
        bind(target);
        ctx_->BufferData(unwrap(target), sizeof(T) * new_data.size(), &new_data[0], unwrap(usage));
        unbind(target);

        size_ = new_data.size();
        target_ = target;
        usage_ = usage;
    }

    void write(const std::vector<T> &new_data) {
        write(new_data, target_, usage_);
    }

    void sub_data(const std::vector<T> &new_data, GLintptr offset, BufTarget target) {
        bind(target);
        ctx_->BufferSubData(unwrap(target), sizeof(T) * offset, sizeof(T) * new_data.size(), &new_data[0]);
        unbind(target);

        target_ = target;
    }

    void sub_data(const std::vector<T> &new_data, GLintptr offset) {
        sub_data(new_data, offset, target_);
    }

    void sub_data(const std::vector<T> &new_data, BufTarget target) {
        sub_data(new_data, 0, target);
    }

    void sub_data(const std::vector<T> &new_data) {
        sub_data(new_data, 0, target_);
    }

    std::vector<T> read(GLintptr offset, GLsizeiptr length, BufTarget target) {
        std::vector<T> data;

        bind(target);
        target_ = target;

        auto glptr = ctx_->MapBufferRange(unwrap(target), sizeof(T) * offset, sizeof(T) * length, GL_MAP_READ_BIT);
        if (!glptr)
            spdlog::error("Failed to glMapBufferRange");
        else {
            data.reserve(length);
            data.assign(reinterpret_cast<T *>(glptr), reinterpret_cast<T *>(glptr) + length);
        }
        ctx_->UnmapBuffer(unwrap(target));
        unbind(target);

        return data;
    }

    std::vector<T> read(GLintptr offset, GLsizeiptr length) {
        return read(offset, length, target_);
    }

    std::vector<T> read(GLsizeiptr length, BufTarget target) {
        return read(0, length, target);
    }

    std::vector<T> read(GLsizeiptr length) {
        return read(0, length, target_);
    }

    std::vector<T> read(BufTarget target) {
        return read(0, size_, target);
    }

    std::vector<T> read() {
        return read(0, size_, target_);
    }

    T *map(GLintptr offset, GLsizeiptr length, BufTarget target) {
        bind(target);
        target_ = target;

        return reinterpret_cast<T *>(
            ctx_->MapBufferRange(
                unwrap(target),
                sizeof(T) * offset,
                sizeof(T) * length,
                GL_MAP_WRITE_BIT
            )
        );
    }

    T *map(GLintptr offset, GLsizeiptr length) {
        return map(offset, length, target_);
    }

    T *map(GLintptr length, BufTarget target) {
        return map(0, length, target);
    }

    T *map(GLintptr length) {
        return map(0, length, target_);
    }

    T *map(BufTarget target) {
        return map(0, size_, target);
    }

    T *map() {
        return map(0, size_, target_);
    }

    void unmap(BufTarget target) {
        ctx_->UnmapBuffer(unwrap(target));
        target_ = target;
    }

    void unmap() {
        unmap(target_);
    }

private:
    BufTarget target_{BufTarget::none};
    BufUsage usage_{BufUsage::none};

    GLsizeiptr size_{0};
};

} // namespace gl

#endif //HADES_GL_STATICBUFFER_HPP
