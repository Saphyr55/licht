#pragma once

#include "licht/rhi/resource.hpp"
#include "licht/rhi/rhi_exports.hpp"
#include "licht/rhi/rhi_fowards.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

struct RHITextureViewDescription {
    RHITexture* texture;
    RHIFormat format;
    RHITextureDimension dimension;
    int32 mip_level = 0;
    int32 mip_level_count = 1;
    int32 array_layer = 0;
    int32 array_layer_count = 1;
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