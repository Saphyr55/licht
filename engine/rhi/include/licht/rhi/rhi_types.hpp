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

enum class RHIShaderStage {
    Vertex,
    Fragment,
    Geometry,
    Tesselation,
    Compute
};

enum class RHICommandBufferUsage {
    None = 0,
    OneTimeSubmit = 1 << 0,
    RenderPassContinue = 1 << 1,
    SimultaneousUse = 1 << 2,
};

inline RHICommandBufferUsage operator|(RHICommandBufferUsage lhs, RHICommandBufferUsage rhs) {
    return static_cast<RHICommandBufferUsage>(
        static_cast<std::underlying_type<RHICommandBufferUsage>::type>(lhs) |
        static_cast<std::underlying_type<RHICommandBufferUsage>::type>(rhs));
}

inline RHICommandBufferUsage operator&(RHICommandBufferUsage lhs, RHICommandBufferUsage rhs) {
    return static_cast<RHICommandBufferUsage>(
        static_cast<std::underlying_type<RHICommandBufferUsage>::type>(lhs) &
        static_cast<std::underlying_type<RHICommandBufferUsage>::type>(rhs));
}

inline RHICommandBufferUsage& operator|=(RHICommandBufferUsage& lhs, RHICommandBufferUsage rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline RHICommandBufferUsage& operator&=(RHICommandBufferUsage& lhs, RHICommandBufferUsage rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline bool rhi_command_buffer_usage_has_flag(RHICommandBufferUsage value, RHICommandBufferUsage flag) {
    return (value & flag) != RHICommandBufferUsage::None;
}

enum class RHIQueueType {
    Unknown,
    Graphics,
    Compute,
    Transfer,
};

enum class RHIVertexInputRate {
    Vertex,
    Instance,
};

enum class RHIBufferUsage : uint8 {
    None = 0,
    Vertex = 1 << 0,
    Index = 1 << 1,
    Uniform = 1 << 2,
    Storage = 1 << 3,
    TransferSrc = 1 << 4,
    TransferDst = 1 << 5
};

inline RHIBufferUsage operator|(RHIBufferUsage a, RHIBufferUsage b) {
    return static_cast<RHIBufferUsage>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline RHIBufferUsage operator&(RHIBufferUsage a, RHIBufferUsage b) {
    return static_cast<RHIBufferUsage>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline RHIBufferUsage& operator|=(RHIBufferUsage& a, RHIBufferUsage b) {
    a = a | b;
    return a;
}

inline RHIBufferUsage& operator&=(RHIBufferUsage& a, RHIBufferUsage b) {
    a = a & b;
    return a;
}

inline bool rhi_buffer_usage_any(RHIBufferUsage usage) {
    return static_cast<uint8_t>(usage) != 0;
}

enum class RHIBufferMemoryUsage {
    Device,
    Host,
};

enum class RHIAccessMode {
    Private,
    Shared
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