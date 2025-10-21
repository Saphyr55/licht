#pragma once

#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/frame_context.hpp"

namespace licht {

class RHISwapchain {
public:
    virtual void acquire_next_frame(RHIFrameContext& context) = 0;

    virtual uint32 get_width() = 0;

    virtual uint32 get_height() = 0;

    virtual RHIFormat get_format() = 0;

    virtual const Array<RHITextureView*>& get_texture_views() const = 0;
};

}  //namespace licht