#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {
    
enum class RHITextureFormat {
    RGBA8,
    BGRA8,
    Depth24Stencil8
};

class RHITexture {

};

struct RHITextureDescription {
    RHIFormat format;
};

using RHITextureHandle = SharedRef<RHITexture>;

struct RHITextureViewDescription {
    RHITextureHandle texture;
    RHIFormat format;
};

class RHITextureView {

};

using RHITextureViewHandle = SharedRef<RHITextureView>;



}