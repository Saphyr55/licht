#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

class RHITexture {
public:
    virtual ~RHITexture() = default;
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
public:
    virtual ~RHITextureView() = default;
};

using RHITextureViewHandle = SharedRef<RHITextureView>;

}  //namespace licht