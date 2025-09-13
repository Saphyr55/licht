#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/rhi.hpp"

namespace licht {

struct RHIAttachmentDescription {
    RHIFormat format;
};

struct RHIRenderPassDescription {
    Array<RHIAttachmentDescription> attachment_decriptions;
};

class RHIRenderPass {
public:
};

using RHIRenderPassHandle = SharedRef<RHIRenderPass>;

}