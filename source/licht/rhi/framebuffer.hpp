#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/texture.hpp"

namespace licht {

struct RHIFramebufferDescription {
    RHIRenderPassHandle render_pass;
    Array<RHITextureViewHandle> attachments;
    uint32 width;
    uint32 height;
    uint32 layers = 1;
};

class RHIFramebuffer {
public:

    RHIFramebuffer();
    virtual ~RHIFramebuffer() = default;
};

using RHIFramebufferHandle = SharedRef<RHIFramebuffer>;

}  //namespace licht