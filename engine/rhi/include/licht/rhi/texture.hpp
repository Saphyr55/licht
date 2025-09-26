#pragma once

#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/rhi_fowards.hpp"

namespace licht {

class RHITexture {
public:
    virtual ~RHITexture() = default;
};

struct RHITextureDescription {
    RHIFormat format;
};


struct RHITextureViewDescription {
    RHITextureHandle texture;
    RHIFormat format;
};

class RHITextureView {
public:
    virtual ~RHITextureView() = default;
};

}  //namespace licht
