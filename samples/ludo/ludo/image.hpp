#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

class Image {
public:
    static SharedRef<Image> load(StringRef filepath);

    inline size_t stride() const { return width_ * channels_; }

    inline bool is_empty() const { return data_ == nullptr || width_ == 0 || height_ == 0; }

    inline size_t get_size() const { return size_; }

    inline size_t get_width() const { return width_; }
    
    inline size_t get_heigth() const { return height_; }

    inline size_t get_channel_count() const { return channels_; }

    inline const uint8* data() const { return data_; }
    inline uint8* data() { return data_; }

private:
    friend class ImageLoader;

    uint8* data_ = nullptr;
    size_t width_ = 0;
    size_t height_ = 0;
    size_t channels_ = 0;
    size_t size_ = 0;
};

}  //namespace licht