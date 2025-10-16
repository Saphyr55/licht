#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

struct TextureBuffer {
    Array<uint8> data;
    float32 width;
    float32 height;
    RHIFormat format = RHIFormat::RGB8sRGB;
};

struct Material {
    TextureBuffer diffuse_texture;
    TextureBuffer normal_texture;
};

}