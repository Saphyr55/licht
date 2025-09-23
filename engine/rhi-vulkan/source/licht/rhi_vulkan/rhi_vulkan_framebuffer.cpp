#include "licht/rhi_vulkan/rhi_vulkan_framebuffer.hpp"

#include "licht/core/containers/array.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_texture.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

namespace licht {

RHIVulkanFramebuffer::RHIVulkanFramebuffer(VulkanContext& context, const RHIFramebufferDescription& description)
    : description_(description)
    , context_(context)
    , handle_(VK_NULL_HANDLE) {
}

RHIVulkanFramebuffer::~RHIVulkanFramebuffer() {
    if (handle_) {
        destroy();
    }
}

VkFramebuffer& RHIVulkanFramebuffer::get_handle() {
    return handle_;
}

void RHIVulkanFramebuffer::initialize() {
    RHIVulkanRenderPassRef render_pass = static_ref_cast<RHIVulkanRenderPass>(description_.render_pass);

    Array<VkImageView> attachments(description_.attachments.size());
    for (RHITextureViewHandle texture_view : description_.attachments) {
        RHIVulkanTextureViewRef vulkan_texture_view = static_ref_cast<RHIVulkanTextureView>(texture_view);
        attachments.append(vulkan_texture_view->get_handle());
    }

    VkFramebufferCreateInfo framebuffer_create_info = {};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = render_pass->get_handle();
    framebuffer_create_info.attachmentCount = attachments.size();
    framebuffer_create_info.pAttachments = attachments.data();
    framebuffer_create_info.width = description_.width;
    framebuffer_create_info.height = description_.height;
    framebuffer_create_info.layers = description_.layers;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateFramebuffer(context_.device, &framebuffer_create_info, context_.allocator, &handle_));
}

void RHIVulkanFramebuffer::destroy() {
    VulkanAPI::lvkDestroyFramebuffer(context_.device, handle_, context_.allocator);
    handle_ = VK_NULL_HANDLE;
}


}  //namespace licht