#include "image_loader.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/shared_ref.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace licht {

class ImageDeleter {
public:
    void operator()(Image* image) const noexcept {
        destroy(image);
    }

    void destroy(Image* image) const noexcept {
        if (!image) {
            return;
        }

        if (image->data) {
            stbi_image_free(image->data);
            image->data = nullptr;
        }

        Memory::delete_resource(image);
    }
};

SharedRef<Image> ImageLoader::load(StringRef filepath) {
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

    Image* img = Memory::new_resource<Image>();
    img->data = pixels;
    img->width = static_cast<size_t>(width);
    img->height = static_cast<size_t>(height);
    img->channels = static_cast<size_t>(channels);
    img->size = static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels);

    return SharedRef<Image>(img, ImageDeleter());
}

}  //namespace licht
