#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi/rhi_fowards.hpp"

namespace licht {

struct RHIFramebufferDescription {
    RHIRenderPass* render_pass;
    Array<RHITextureView*> attachments;
    uint32 width;
    uint32 height;
    uint32 layers = 1;
};

class RHIFramebuffer {
public:
    virtual ~RHIFramebuffer() = default;
};


}  //namespace licht