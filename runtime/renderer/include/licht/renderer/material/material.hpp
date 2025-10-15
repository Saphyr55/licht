#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/texture_view.hpp"

namespace licht {

struct TextureBuffer {
    Array<uint8> data;
    float32 width;
    float32 height;
    RHIFormat format = RHIFormat::RGB8sRGB;
};

struct Material {
    TextureBuffer diffuse_texture;
};

}