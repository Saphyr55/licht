#pragma once

#include "licht/rhi/rhi_types.hpp"

namespace licht {

struct RHISamplerDescription {
    RHIFilter mag_filter = RHIFilter::Linear;
    RHIFilter min_filter = RHIFilter::Linear;
    RHISamplerAddressMode address_mode_u = RHISamplerAddressMode::Repeat;
    RHISamplerAddressMode address_mode_v = RHISamplerAddressMode::Repeat;
    RHISamplerAddressMode address_mode_w = RHISamplerAddressMode::Repeat;
    bool normalize = false;
};

class RHISampler {
public:
    inline const RHISamplerDescription& get_description() const {
        return description_;
    }

public:
    virtual ~RHISampler() = default;

protected:
    RHISampler() = default;

protected:
    RHISamplerDescription description_;
};

}  //namespace licht