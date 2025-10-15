#pragma once

#include "licht/rhi/resource.hpp"
#include "licht/rhi/rhi_exports.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

struct RHITextureViewDescription {
    RHITexture* texture = nullptr;
    RHIFormat format = RHIFormat::RGBA8sRGB;
    RHITextureDimension dimension = RHITextureDimension::Dim2D;
    uint32 mip_levels = 1;
};

class LICHT_RHI_API RHITextureView : public RHIResource {
public:
    const RHITextureViewDescription& get_description() const {
        return description_;
    }

    virtual ~RHITextureView() = default;

protected:
    RHITextureView() {}

protected:
    RHITextureViewDescription description_;
};

}  //namespace licht