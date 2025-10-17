#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/containers/option.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

struct RHIColorAttachmentDescription {
    RHIFormat format;
};

struct RHIDepthAttachementDescription {
    RHIFormat format;
};

struct RHIRenderPassDescription {
    Array<RHIColorAttachmentDescription> color_attachment_decriptions;
    Option<RHIDepthAttachementDescription> deph_attachement_description = RHIDepthAttachementDescription(RHIFormat::D24S8);
};

class RHIRenderPass {
public:
    virtual const RHIRenderPassDescription& get_description() const = 0;

    virtual ~RHIRenderPass() = default;
};

}  //namespace licht