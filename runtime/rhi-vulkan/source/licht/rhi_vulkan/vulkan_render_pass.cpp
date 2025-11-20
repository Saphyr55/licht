#include "licht/rhi_vulkan/vulkan_render_pass.hpp"
#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

void VulkanRenderPass::initialize() {
    Array<VkAttachmentDescription> attachments(description_.color_attachment_decriptions.size() + 1);
    for (const RHIColorAttachmentDescription& attachment_description : description_.color_attachment_decriptions) {
        VkAttachmentDescription color_attachment_description = {};
        color_attachment_description.format = vulkan_format_get(attachment_description.format);
        color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment_description.loadOp = vulkan_load_op_get(attachment_description.load_op);
        color_attachment_description.storeOp = vulkan_store_op_get(attachment_description.store_op);
        color_attachment_description.stencilLoadOp = vulkan_load_op_get(attachment_description.stencil_load_op);
        color_attachment_description.stencilStoreOp = vulkan_store_op_get(attachment_description.stencil_store_op);
        color_attachment_description.initialLayout =  vulkan_texture_layout_get(attachment_description.initial_layout);
        color_attachment_description.finalLayout = vulkan_texture_layout_get(attachment_description.final_layout);
        attachments.append(color_attachment_description);
    }

    VkAttachmentReference color_attachment_reference = {};
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_description = {};
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &color_attachment_reference;

    if (description_.deph_attachement_description.has_value()) {
        RHIDepthAttachementDescription& depth_attachement_desc = *description_.deph_attachement_description;

        VkImageLayout layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        if (rhi_format_is_depth_stencil(depth_attachement_desc.format)) {
            layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        } else if (rhi_format_is_depth(depth_attachement_desc.format)) {
            layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        } else {
            LCRASH("Not supported format for depth or depth-stencil.");
        }

        VkAttachmentDescription deph_attachement_description = {};
        deph_attachement_description.format = vulkan_format_get(depth_attachement_desc.format);
        deph_attachement_description.samples = VK_SAMPLE_COUNT_1_BIT;
        deph_attachement_description.loadOp = vulkan_load_op_get(depth_attachement_desc.load_op);;
        deph_attachement_description.storeOp = vulkan_store_op_get(depth_attachement_desc.store_op);;
        deph_attachement_description.stencilLoadOp = vulkan_load_op_get(depth_attachement_desc.stencil_load_op);
        deph_attachement_description.stencilStoreOp = vulkan_store_op_get(depth_attachement_desc.stencil_store_op);
        deph_attachement_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        deph_attachement_description.finalLayout = layout;

        VkAttachmentReference deph_attachement_reference = {};
        deph_attachement_reference.attachment = 1;
        deph_attachement_reference.layout = layout;

        subpass_description.pDepthStencilAttachment = &deph_attachement_reference;
        attachments.append(deph_attachement_description);
    }

    VkSubpassDependency subpass_dependency = {};
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency depth_subpass_dependency = {};
    depth_subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depth_subpass_dependency.dstSubpass = 0;
    depth_subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_subpass_dependency.srcAccessMask = 0;
    depth_subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_subpass_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    Array<VkSubpassDependency> dependencies(attachments.size());
    for (size_t i = 0; i < attachments.size() - 1; i++) {
        dependencies.append(subpass_dependency);
    }

    if (description_.deph_attachement_description.has_value()) {
        dependencies.append(depth_subpass_dependency);
    }

    VkRenderPassCreateInfo render_pass_info_create_info = {};
    render_pass_info_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info_create_info.attachmentCount = attachments.size();
    render_pass_info_create_info.pAttachments = attachments.data();
    render_pass_info_create_info.subpassCount = 1;
    render_pass_info_create_info.pSubpasses = &subpass_description;
    render_pass_info_create_info.dependencyCount = dependencies.size();
    render_pass_info_create_info.pDependencies = dependencies.data();

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateRenderPass(context_.device,
                                                      &render_pass_info_create_info,
                                                      context_.allocator,
                                                      &render_pass_));
}

void VulkanRenderPass::destroy() {
    VulkanAPI::lvkDestroyRenderPass(context_.device, render_pass_, context_.allocator);
}

}  //namespace licht