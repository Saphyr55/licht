#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/containers/option.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

struct RHIColorAttachmentDescription {
    RHIFormat format;
    
    RHIAttachmentLoadOp load_op = RHIAttachmentLoadOp::Load_OP_Clear;
    RHIAttachmentStoreOp store_op = RHIAttachmentStoreOp::Store_OP_Store;
    
    RHIAttachmentLoadOp stencil_load_op = RHIAttachmentLoadOp::Load_OP_DontCare;
    RHIAttachmentStoreOp stencil_store_op = RHIAttachmentStoreOp::Store_OP_DontCare;
    
    RHITextureLayout initial_layout = RHITextureLayout::Undefined;
    RHITextureLayout final_layout = RHITextureLayout::Present;
};

struct RHIDepthAttachementDescription {
    RHIFormat format;
    
    RHIAttachmentLoadOp load_op = RHIAttachmentLoadOp::Load_OP_Clear;
    RHIAttachmentStoreOp store_op = RHIAttachmentStoreOp::Store_OP_DontCare;

    RHIAttachmentLoadOp stencil_load_op = RHIAttachmentLoadOp::Load_OP_Clear;
    RHIAttachmentStoreOp stencil_store_op = RHIAttachmentStoreOp::Store_OP_DontCare;
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