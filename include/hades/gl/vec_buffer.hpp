#ifndef HADES_GL_VEC_BUFFER_HPP
#define HADES_GL_VEC_BUFFER_HPP

#include "hades/gl/buffer_base.hpp"
#include "spdlog/spdlog.h"

#include <algorithm>
#include <iterator>
#include <vector>

namespace gl {

template<typename T>
class VecBuffer : public BufferBase {
public:
    VecBuffer(GladGLContext *ctx, BufTarget target, BufUsage usage, std::size_t initial_size, bool front_to_back)
        : BufferBase(ctx), target_(target), usage_(usage), front_to_back_(front_to_back) {

        gen_id_();

        data_.resize(initial_size);
        if (front_to_back_) {
            front_ = initial_size;
            back_ = initial_size;
        }
    }

    ~VecBuffer() override = default;

    // Copy constructors don't make sense for OpenGL objects
    VecBuffer(const VecBuffer &) = delete;
    VecBuffer &operator=(const VecBuffer &) = delete;

    VecBuffer(VecBuffer &&other) noexcept: BufferBase(std::move(other)) {
        target_ = other.target_;
        usage_ = other.usage_;
        data_ = other.data_;
        front_ = other.front_;
        back_ = other.back_;
        front_to_back_ = other.front_to_back_;
        gl_bufsize_ = other.gl_bufsize_;
        gl_bufpos_ = other.gl_bufpos_;

        other.target_ = BufTarget::none;
        other.usage_ = BufUsage::none;
        other.data_.clear();
        other.front_ = 0;
        other.back_ = 0;
        other.front_to_back_ = false;
        other.gl_bufsize_ = 0;
        other.gl_bufpos_ = 0;
    }

    VecBuffer &operator=(VecBuffer &&other) noexcept {
        if (this != &other) {
            BufferBase::operator=(std::move(other));

            target_ = other.target_;
            usage_ = other.usage_;
            data_ = other.data_;
            front_ = other.front_;
            back_ = other.back_;
            front_to_back_ = other.front_to_back_;
            gl_bufsize_ = other.gl_bufsize_;
            gl_bufpos_ = other.gl_bufpos_;

            other.target_ = BufTarget::none;
            other.usage_ = BufUsage::none;
            other.data_.clear();
            other.front_ = 0;
            other.back_ = 0;
            other.front_to_back_ = false;
            other.gl_bufsize_ = 0;
            other.gl_bufpos_ = 0;
        }
        return *this;
    }

    void clear() {
        if (front_to_back_) {
            front_ = data_.size();
            back_ = data_.size();
            gl_bufpos_ = front_;

        } else {
            front_ = 0u;
            back_ = 0u;
            gl_bufpos_ = back_;
        }
    }

    std::size_t front() {
        return front_;
    }

    std::size_t size() {
        return back_ - front_;
    }

    void add(const std::vector<T> &new_data) {
        add_(new_data.begin(), new_data.end());
    }

    void add(std::initializer_list<T> new_data) {
        add_(new_data.begin(), new_data.end());
    }

    void sync() {
        if (gl_bufsize_ < data_.size()) {
            bind(target_);
            ctx_->BufferData(
                unwrap(target_),
                sizeof(T) * data_.size(),
                &data_[0],
                unwrap(usage_)
            );
            unbind(target_);

            gl_bufsize_ = data_.size();
            gl_bufpos_ = front_to_back_ ? front_ : back_;

        } else {
            if (front_to_back_ && gl_bufpos_ > front_) {
                bind(target_);
                ctx_->BufferSubData(
                    unwrap(target_),
                    sizeof(T) * front_,
                    sizeof(T) * (gl_bufpos_ - front_),
                    &data_[0] + front_
                );
                unbind(target_);

                gl_bufpos_ = front_;

            } else if (!front_to_back_ && gl_bufpos_ < back_) {
                bind(target_);
                ctx_->BufferSubData(
                    unwrap(target_),
                    sizeof(T) * gl_bufpos_,
                    sizeof(T) * (back_ - gl_bufpos_),
                    &data_[0] + gl_bufpos_
                );
                unbind(target_);

                gl_bufpos_ = back_;
            }
        }
    }

private:
    BufTarget target_{BufTarget::none};
    BufUsage usage_{BufUsage::none};

    std::vector<T> data_{};
    std::size_t front_{0}, back_{0};
    bool front_to_back_{false};

    GLuint gl_bufsize_{0u}, gl_bufpos_{0u};

    template<typename InputIt>
    void add_(InputIt begin, InputIt end) {
        std::size_t size = std::distance(begin, end);
        if (front_to_back_) {
            while (front_ + 1 <= size) {
                front_ += data_.size();
                back_ += data_.size();
                data_.reserve(data_.size() * 2);
                std::copy(data_.begin(), data_.end(), std::back_inserter(data_));
            }
            front_ -= size;
            std::copy(begin, end, data_.begin() + front_);

        } else {
            while (back_ + size >= data_.size())
                data_.resize(data_.size() * 2);
            std::copy(begin, end, data_.begin() + back_);
            back_ += size;
        }
    }
};

} // namespace gl

#endif //HADES_GL_VEC_BUFFER_HPP
