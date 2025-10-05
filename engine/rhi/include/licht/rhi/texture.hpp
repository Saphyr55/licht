#pragma once

#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/rhi_fowards.hpp"
#include "licht/rhi/resource.hpp"

namespace licht {

struct RHITextureBarrier {
    RHITexture* texture = nullptr;
    RHITextureLayout old_layout = RHITextureLayout::Undefined;
    RHITextureLayout new_layout = RHITextureLayout::Undefined;
};

struct RHITextureDescription {
    RHIFormat format = RHIFormat::RGBA32Float;
    RHITextureUsageFlags usage = RHITextureUsageFlags::ColorAttachment;
    RHISharingMode sharing_mode = RHISharingMode::Private;
    RHIMemoryUsage memory_usage = RHIMemoryUsage::Device;
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

class RHITexture : public RHIResource {
public:
    virtual void bind() = 0;

    virtual const RHITextureDescription& get_description() const = 0;

    virtual ~RHITexture() = default;
};

class RHITextureView : public RHIResource {
public:
    virtual ~RHITextureView() = default;
};

}  //namespace licht
