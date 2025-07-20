#include "vulkan_render_pass.hpp"

#include "licht/core/defines.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

void vulkan_render_pass_init(VulkanContext* p_context) {
    LCHECK(p_context);

    VkSubpassDependency subpass_dependency = {};
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription color_attachment_description = {};
    color_attachment_description.format = p_context->swapchain_format;
    color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_reference = {};
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_description = {};
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &color_attachment_reference;

    VkRenderPassCreateInfo render_pass_info_create_info = {};
    render_pass_info_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info_create_info.attachmentCount = 1;
    render_pass_info_create_info.pAttachments = &color_attachment_description;
    render_pass_info_create_info.subpassCount = 1;
    render_pass_info_create_info.pSubpasses = &subpass_description;
    render_pass_info_create_info.dependencyCount = 1;
    render_pass_info_create_info.pDependencies = &subpass_dependency;

    LICHT_VULKAN_CHECK(p_context->api.licht_vkCreateRenderPass(p_context->device, &render_pass_info_create_info, p_context->allocator, &p_context->render_pass))
}

void vulkan_render_pass_begin(VulkanContext* p_context, VkCommandBuffer p_command_buffer, uint32 p_image_index) {
    LCHECK(p_context);

    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = p_context->render_pass;
    render_pass_begin_info.framebuffer = p_context->framebuffers[p_image_index];
    render_pass_begin_info.renderArea.offset = {0, 0};
    render_pass_begin_info.renderArea.extent = p_context->swapchain_extent;

    VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clear_color;

    p_context->api.licht_vkCmdBeginRenderPass(p_command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void vulkan_render_pass_end(VulkanContext* p_context, VkCommandBuffer p_command_buffer) {
    LCHECK(p_context);

    p_context->api.licht_vkCmdEndRenderPass(p_command_buffer);
}

void vulkan_render_pass_destroy(VulkanContext* p_context) {
    LCHECK(p_context);

    p_context->api.licht_vkDestroyRenderPass(p_context->device, p_context->render_pass, p_context->allocator);
}

}  //namespace licht