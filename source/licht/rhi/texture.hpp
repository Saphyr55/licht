#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/resource.hpp"

namespace licht {
    
enum class RHITextureFormat {
    RGBA8,
    BGRA8,
    Depth24Stencil8
};

class RHITexture : public RHIResource {

};

class RHITextureView {

};

using RHITextureHandle = SharedRef<RHITexture>;

using RHITextureViewHandle = SharedRef<RHITextureView>;

}