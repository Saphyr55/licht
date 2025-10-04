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

enum class RHITextureUsage : uint8 {
    ColorAttachment = 1 << 0,
    DepthStencil = 1 << 1,
    Sampled = 1 << 2,
    TransferSrc = 1 << 3,
    TransferDst = 1 << 4
};
LICHT_ENUM_FLAGS(RHITextureUsage)

enum class RHIShaderStage {
    Vertex,
    Fragment,
    Geometry,
    Tesselation,
    Compute
};

enum class RHIDescriptorSetType {
    Uniform,
    Sampler,
    StorageBuffer,
    StorageImage,
    CombinedImageSampler
};

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

enum class RHICommandBufferUsage : uint8 {
    None = 0,
    OneTimeSubmit = 1 << 0,
    RenderPassContinue = 1 << 1,
    SimultaneousUse = 1 << 2,
};
LICHT_ENUM_FLAGS(RHICommandBufferUsage)

enum class RHIBufferUsage : uint8 {
    None = 0,
    Vertex = 1 << 0,
    Index = 1 << 1,
    Uniform = 1 << 2,
    Storage = 1 << 3,
    TransferSrc = 1 << 4,
    TransferDst = 1 << 5
};
LICHT_ENUM_FLAGS(RHIBufferUsage)

enum class TextureUsageFlags : uint8 {
    ColorAttachment = 1 << 0,
    DepthStencilAttachment = 1 << 1,
    Sampled = 1 << 2,
    Storage = 1 << 3,
    TransferSrc = 1 << 4,
    TransferDst = 1 << 5
};
LICHT_ENUM_FLAGS(TextureUsageFlags)

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

inline bool rhi_command_buffer_usage_has_flag(RHICommandBufferUsage value, RHICommandBufferUsage flag) {
    return (value & flag) == flag;
}

inline bool rhi_buffer_usage_any(RHIBufferUsage usage) {
    return static_cast<uint8>(usage) != 0;
}

}  //namespace licht