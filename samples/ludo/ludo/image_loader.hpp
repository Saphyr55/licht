#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/linear_allocator.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

struct Image {
    uint8* data = nullptr;
    size_t width = 0;
    size_t height = 0;
    size_t channels = 0;
    size_t size = 0;

    inline size_t stride() const noexcept { return width * channels; }

    inline bool empty() const noexcept { return data == nullptr || width == 0 || height == 0; }
};

class ImageLoader {
public:
    static SharedRef<Image> load(StringRef filepath);
};

}  //namespace licht