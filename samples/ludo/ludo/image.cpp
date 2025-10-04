#include "image.hpp"
#include "licht/core/containers/hash_map.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/shared_ref.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace licht {

static HashMap<StringRef, Image> image_registry_;

Image* Image::load(StringRef name, StringRef filepath) {
    if (filepath.empty()) {
        return nullptr;
    }

    int32 width = 0;
    int32 height = 0;
    int32 channels = 0;
    stbi_uc* pixels = stbi_load(filepath.data(), &width, &height, &channels, STBI_rgb_alpha);

    if (!pixels) {
        return nullptr;
    }

    Image img;
    img.name_ = name;
    img.data_ = pixels;
    img.width_ = static_cast<size_t>(width);
    img.height_ = static_cast<size_t>(height);
    img.channels_ = static_cast<size_t>(channels);
    img.size_ = static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels);

    Image* img_ptr = &image_registry_.put(name, std::move(img)).value;

    return img_ptr;
}

void Image::unload(Image* image) {
    stbi_image_free(image->data());
    image_registry_.remove(image->get_name());
}

}  //namespace licht
