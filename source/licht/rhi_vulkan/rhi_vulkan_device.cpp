#include "rhi_vulkan_device.hpp"
#include "licht/core/collection/array.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_framebuffer.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_swapchain.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_texture.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

RHIGraphicsPipelineHandle create_graphics_pipeline(RHIPipelineDescription description) {
    
}

RHISwapchainHandle RHIVulkanDevice::create_swapchain(RHIRenderSurfaceHandle render_surface) {
    SharedRef<RHIVulkanRenderSurface> vulkan_render_surface = static_ref_cast<RHIVulkanRenderSurface>(render_surface);
    SharedRef<RHIVulkanSwapchain> swapchain = new_ref<RHIVulkanSwapchain>(context_, vulkan_render_surface);
    swapchain->initialize();
    return swapchain;
}

void RHIVulkanDevice::recreate_swapchain(RHISwapchainHandle swapchain) {
    SharedRef<RHIVulkanSwapchain> vulkan_swapchain = static_ref_cast<RHIVulkanSwapchain>(swapchain);
    
    LICHT_VULKAN_CHECK(VulkanAPI::lvkDeviceWaitIdle(context_.device));

    // TODO: Destroy framebuffers

    vulkan_swapchain->destroy();
    vulkan_swapchain->initialize();

    // TODO: Init framebuffers
}

RHIFramebufferHandle RHIVulkanDevice::create_framebuffer(RHIRenderPassHandle render_pass,
                                                         const Array<RHITextureViewHandle>& attachements,
                                                         const RHIFramebufferDescription& description) {

    SharedRef<RHIVulkanFramebuffer> framebuffer = new_ref<RHIVulkanFramebuffer>(description);
    SharedRef<RHIVulkanRenderPass> render_pass = static_ref_cast<RHIVulkanRenderPass>(render_pass);

    Array<VkImageView> attachments = attachements.map<VkImageView>([](const RHITextureViewHandle& texture_view) {
        SharedRef<RHIVulkanTextureView> vulkan_texture_view = static_ref_cast<RHIVulkanTextureView>(texture_view);
        return vulkan_texture_view->get_handle();
    });

    VkFramebufferCreateInfo framebuffer_create_info = {};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = render_pass->get_handle();
    framebuffer_create_info.attachmentCount = attachements.size();
    framebuffer_create_info.pAttachments = attachments.data();
    framebuffer_create_info.width = description.width;
    framebuffer_create_info.height = description.height;
    framebuffer_create_info.layers = description.layers;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateFramebuffer(context_.device, &framebuffer_create_info, context_.allocator, &framebuffer->get_handle()));

    return framebuffer;
}

void RHIVulkanDevice::destroy_framebuffer(RHIFramebufferHandle framebuffer) {
    SharedRef<RHIVulkanFramebuffer> vulkan_framebuffer = static_ref_cast<RHIVulkanFramebuffer>(framebuffer);
    VulkanAPI::lvkDestroyFramebuffer(context_.device, vulkan_framebuffer->get_handle(), context_.allocator);
}

RHIVulkanDevice::RHIVulkanDevice(VulkanContext& context)
    : context_(context) {
}

}  //namespace licht