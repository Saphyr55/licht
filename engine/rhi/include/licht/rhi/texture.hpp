#pragma once

#include "licht/rhi/rhi_exports.hpp"
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
    RHITextureDimension dimension = RHITextureDimension::Dim2D;
    size_t width = 8;
    size_t height = 8;
    size_t depth = 1;
    size_t mip_levels = 1;
    size_t array_layers = 1;
};

class LICHT_RHI_API RHITexture : public RHIResource {
public:
    const RHITextureDescription& get_description() const;

    virtual void bind() = 0;

    virtual ~RHITexture() = default;

protected:
    RHITexture() = default;

protected:
    RHITextureDescription description_;
};

LICHT_RHI_API void rhi_transition_texture(RHIDeviceRef device, const RHITextureBarrier& barrier);

}  //namespace licht
