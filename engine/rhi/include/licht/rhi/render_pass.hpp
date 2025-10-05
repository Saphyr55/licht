#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

struct RHIAttachmentDescription {
    RHIFormat format;
};

struct RHIRenderPassDescription {
    Array<RHIAttachmentDescription> attachment_decriptions;
};

class RHIRenderPass {
public:
    virtual ~RHIRenderPass() = default;
};


}