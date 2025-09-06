#pragma once

#include "licht/core/defines.hpp"

namespace licht {

struct Viewport {
    float32 x = 0.0f;
    float32 y = 0.0f;
    float32 width = 0.0f;
    float32 height = 0.0f;
    float32 min_depth = 0.0f;
    float32 max_depth = 0.0f;
};

struct Rect2D {
    float32 x = 0.0f;
    float32 y = 0.0f;
    float32 width = 0.0f;
    float32 height = 0.0f;
};

enum class RHIFormat {
    Undefined,

    // 8-bit UNORM
    R8,
    RG8,
    RGB8,
    RGBA8,
    BGRA8,
    R8sRGB,
    RG8sRGB,
    RGB8sRGB,
    RGBA8sRGB,
    BGRA8sRGB,

    // 16-bit
    R16,
    RG16,
    RGB16,
    RGBA16,
    R16Float,
    RG16Float,
    RGB16Float,
    RGBA16Float,

    // 32-bit
    R32Uint,
    RG32Uint,
    RGB32Uint,
    RGBA32Uint,
    R32Sint,
    RG32Sint,
    RGB32Sint,
    RGBA32Sint,
    R32Float,
    RG32Float,
    RGB32Float,
    RGBA32Float,

    // Packed
    RGB10A2,
    R11G11B10Float,
    RGB9E5,

    // Depth / Stencil
    D16,
    D24,
    D32,
    D24S8,
    D32S8,

    // Compressed (examples)
    BC1,
    BC3,
    BC5,
    BC7,
    ETC2_RGB8,
    ETC2_RGBA8,
    ASTC_4x4,
    ASTC_8x8,
};

}  //namespace licht