#pragma once

#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/rhi_fowards.hpp"
#include "licht/rhi/resource.hpp"

namespace licht {

class RHITexture : public RHIResource {
public:
    virtual void bind() = 0;

    virtual ~RHITexture() = default;
};

struct RHITextureDescription {
    RHIFormat format = RHIFormat::R32Float;
    size_t width = 8;
    size_t height = 8;
    size_t depth = 1;
    size_t mipmap_levels = 1;
    size_t layers_count = 1;
};

struct RHITextureViewDescription {
    RHITexture* texture;
    RHIFormat format;
};

class RHITextureView : public RHIResource {
public:
    virtual ~RHITextureView() = default;
};

}  //namespace licht
