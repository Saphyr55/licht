#include "rhi_vulkan_framebuffer.hpp"

#include "licht/core/collection/array.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_texture.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

namespace licht {

VkFramebuffer& RHIVulkanFramebuffer::get_handle() {
    return handle_;
}

RHIVulkanFramebuffer::RHIVulkanFramebuffer() {
}

void vulkan_framebuffer_init(VulkanContext& context, RHIVulkanFramebufferRef framebuffer, const RHIFramebufferDescription& description) {
    RHIVulkanRenderPassRef render_passs = static_ref_cast<RHIVulkanRenderPass>(description.render_pass);

    Array<VkImageView> attachments(description.attachments.size());
    for (RHITextureViewHandle texture_view : description.attachments) {
        RHIVulkanTextureViewRef vulkan_texture_view = static_ref_cast<RHIVulkanTextureView>(texture_view);
        attachments.append(vulkan_texture_view->get_handle());
    }

    VkFramebufferCreateInfo framebuffer_create_info = {};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = render_passs->get_handle();
    framebuffer_create_info.attachmentCount = attachments.size();
    framebuffer_create_info.pAttachments = attachments.data();
    framebuffer_create_info.width = description.width;
    framebuffer_create_info.height = description.height;
    framebuffer_create_info.layers = description.layers;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateFramebuffer(context.device, &framebuffer_create_info, context.allocator, &framebuffer->get_handle()));
}

void vulkan_framebuffer_destroy(VulkanContext& context, RHIVulkanFramebufferRef framebuffer) {
    VulkanAPI::lvkDestroyFramebuffer(context.device, framebuffer->get_handle(), context.allocator);
}

}  //namespace licht