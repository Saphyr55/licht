#include "vulkan_framebuffer.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

void vulkan_framebuffers_init(VulkanContext* p_context) {
    LCHECK(p_context)

    p_context->framebuffers.resize(p_context->swapchain_image_views.size());

    for (usize i = 0; i < p_context->swapchain_image_views.size(); i++) {
        VkImageView attachments[] = {p_context->swapchain_image_views[i]};

        VkFramebufferCreateInfo framebuffer_create_info = {};
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.renderPass = p_context->render_pass;
        framebuffer_create_info.attachmentCount = 1;
        framebuffer_create_info.pAttachments = attachments;
        framebuffer_create_info.width = p_context->swapchain_extent.width;
        framebuffer_create_info.height = p_context->swapchain_extent.height;
        framebuffer_create_info.layers = 1;

        LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateFramebuffer(p_context->device->get_handle(), &framebuffer_create_info, p_context->allocator, &p_context->framebuffers[i]));
    }

}

void vulkan_framebuffers_destroy(VulkanContext* p_context) {
    for (VkFramebuffer framebuffer : p_context->framebuffers) {
        VulkanAPI::lvkDestroyFramebuffer(p_context->device->get_handle(), framebuffer, p_context->allocator);
    }
}

}  //namespace licht