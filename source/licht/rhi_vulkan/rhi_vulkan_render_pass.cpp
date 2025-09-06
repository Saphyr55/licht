#include "licht/rhi_vulkan/rhi_vulkan_render_pass.hpp"
#include "licht/core/collection/array.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

void vulkan_render_pass_init(VulkanContext& context, VkRenderPass* render_pass, const RHIRenderPassDescription& description) {
    Array<VkAttachmentDescription> attachments(description.attachment_decriptions.size());
    for (const RHIAttachmentDescription& attachment_description : description.attachment_decriptions) {
        VkAttachmentDescription color_attachment_description = {};
        color_attachment_description.format = vulkan_format_get(attachment_description.format);
        color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachments.append(color_attachment_description);
    }

    VkAttachmentReference color_attachment_reference = {};
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_description = {};
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &color_attachment_reference;

    VkSubpassDependency subpass_dependency = {};
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info_create_info = {};
    render_pass_info_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info_create_info.attachmentCount = 1;
    render_pass_info_create_info.pAttachments = attachments.data();
    render_pass_info_create_info.subpassCount = 1;
    render_pass_info_create_info.pSubpasses = &subpass_description;
    render_pass_info_create_info.dependencyCount = 1;
    render_pass_info_create_info.pDependencies = &subpass_dependency;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateRenderPass(context.device, &render_pass_info_create_info, context.allocator, render_pass))
}

void vulkan_render_pass_destroy(VulkanContext& context, VkRenderPass render_pass) {
    VulkanAPI::lvkDestroyRenderPass(context.device, render_pass, context.allocator);
}

}  //namespace licht