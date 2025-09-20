#include "licht/rhi_vulkan/rhi_vulkan_device.hpp"

#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/fence.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/semaphore.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/rhi_sync.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_command_queue.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_framebuffer.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_pipeline.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_swapchain.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_texture.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_buffer.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

void RHIVulkanDevice::wait_fence(RHIFenceHandle fence) {
    if (fence->is_signaled()) {
        return;
    }

    SharedRef<RHIVulkanFence> rhi_vk_fence = static_ref_cast<RHIVulkanFence>(fence);
    const VkFence& vkfence = rhi_vk_fence->get_handle();

    VkResult result_wait = VulkanAPI::lvkWaitForFences(context_.device, 1, &vkfence, VK_TRUE, UINT64_MAX);
    switch (result_wait) {
        case VK_SUCCESS: {
            rhi_vk_fence->set_signaled(true);
            return;
        }
        default: {
            LICHT_VULKAN_CHECK(result_wait)
            break;
        }
    }
}

void RHIVulkanDevice::reset_fence(RHIFenceHandle fence) {
    if (!fence->is_signaled()) {
        return;
    }

    SharedRef<RHIVulkanFence> rhi_vk_fence = static_ref_cast<RHIVulkanFence>(fence);
    VkFence& vkfence = rhi_vk_fence->get_handle();
    LICHT_VULKAN_CHECK(VulkanAPI::lvkResetFences(context_.device, 1, &vkfence));

    rhi_vk_fence->set_signaled(false);
}

void RHIVulkanDevice::wait_idle() {
    LICHT_VULKAN_CHECK(VulkanAPI::lvkDeviceWaitIdle(context_.device));
}

RHICommandAllocatorRef RHIVulkanDevice::create_command_allocator(const RHICommandAllocatorDescription& description) {
    SharedRef<RHIVulkanCommandAllocator> vulkan_command_allocator = new_ref<RHIVulkanCommandAllocator>(context_, description);
    vulkan_command_allocator->initialize_command_pool();
    vulkan_command_allocator->allocate_command_buffers();
    return vulkan_command_allocator;
}

void RHIVulkanDevice::destroy_command_allocator(RHICommandAllocatorRef command_allocator) {
    SharedRef<RHIVulkanCommandAllocator> vulkan_command_allocator = static_ref_cast<RHIVulkanCommandAllocator>(command_allocator);
    vulkan_command_allocator->destroy();
}

RHITextureHandle RHIVulkanDevice::create_texture(const RHITextureDescription& description) {
    RHIVulkanTextureRef texture = new_ref<RHIVulkanTexture>();
    VkImageCreateInfo image_create_info = {};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.format = vulkan_format_get(description.format);
    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateImage(context_.device, &image_create_info, context_.allocator, &texture->get_handle()))
    return texture;
}

void RHIVulkanDevice::destroy_texture(RHITextureHandle texture) {
    RHIVulkanTextureRef vktexture = static_ref_cast<RHIVulkanTexture>(texture);
    VulkanAPI::lvkDestroyImage(context_.device, vktexture->get_handle(), context_.allocator);
}

RHIBufferHandle RHIVulkanDevice::create_buffer(RHIBufferDescription description) {
    RHIVulkanBufferRef buffer = new_ref<RHIVulkanBuffer>(context_, description);
    buffer->initialize();
    return buffer;
}

void RHIVulkanDevice::destroy_buffer(RHIBufferHandle buffer) {
    RHIVulkanBufferRef vkbuffer = static_ref_cast<RHIVulkanBuffer>(buffer);
    vkbuffer->destroy();
}

RHITextureViewHandle RHIVulkanDevice::create_texture_view(const RHITextureViewDescription& description) {
    RHIVulkanTextureViewRef texture_view = new_ref<RHIVulkanTextureView>();

    RHIVulkanTextureRef texture = static_ref_cast<RHIVulkanTexture>(description.texture);

    VkImageViewCreateInfo image_view_create_info = {};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = texture->get_handle();
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = vulkan_format_get(description.format);
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateImageView(context_.device, &image_view_create_info, context_.allocator, &texture_view->get_handle()))

    return texture_view;
}

void RHIVulkanDevice::destroy_texture_view(RHITextureViewHandle texture_view) {
    RHIVulkanTextureViewRef vk_texture_view = static_ref_cast<RHIVulkanTextureView>(texture_view);
    VulkanAPI::lvkDestroyImageView(context_.device, vk_texture_view->get_handle(), context_.allocator);
}

RHIRenderPassHandle RHIVulkanDevice::create_render_pass(const RHIRenderPassDescription& description) {
    RHIVulkanRenderPassRef render_pass = new_ref<RHIVulkanRenderPass>(context_);
    vulkan_render_pass_init(context_, &render_pass->get_handle(), description);
    return render_pass;
}

void RHIVulkanDevice::destroy_render_pass(RHIRenderPassHandle render_pass) {
    RHIVulkanRenderPassRef vulkan_render_pass = static_ref_cast<RHIVulkanRenderPass>(render_pass);
    vulkan_render_pass_destroy(context_, vulkan_render_pass->get_handle());
}

RHIPipelineHandle RHIVulkanDevice::create_graphics_pipeline(const RHIPipelineDescription& description) {
    RHIVulkanPipelineRef graphics_pipeline = new_ref<RHIVulkanPipeline>(context_, description);
    graphics_pipeline->initialize();
    return graphics_pipeline;
}

void RHIVulkanDevice::destroy_graphics_pipeline(RHIPipelineHandle pipeline) {
    RHIVulkanPipelineRef graphics_pipeline = static_ref_cast<RHIVulkanPipeline>(pipeline);
    graphics_pipeline->destroy();
}

RHISwapchainHandle RHIVulkanDevice::create_swapchain(uint32 width, uint32 height) {
    RHIVulkanSwapchainRef swapchain = new_ref<RHIVulkanSwapchain>(context_, width, height);
    swapchain->initialize();
    return swapchain;
}

void RHIVulkanDevice::recreate_swapchain(RHISwapchainHandle swapchain, uint32 width, uint32 height) {
    SharedRef<RHIVulkanSwapchain> vulkan_swapchain = static_ref_cast<RHIVulkanSwapchain>(swapchain);
    vulkan_swapchain->set_extent({width, height});
    vulkan_swapchain->destroy();
    vulkan_swapchain->initialize();
}

void RHIVulkanDevice::destroy_swapchain(RHISwapchainHandle swapchain) {
    LCHECK(swapchain);
    SharedRef<RHIVulkanSwapchain> vulkan_swapchain = static_ref_cast<RHIVulkanSwapchain>(swapchain);
    vulkan_swapchain->destroy();
}

RHIFramebufferHandle RHIVulkanDevice::create_framebuffer(RHIRenderPassHandle render_pass,
                                                         const RHIFramebufferDescription& description) {
    LCHECK(render_pass);

    SharedRef<RHIVulkanFramebuffer> framebuffer = new_ref<RHIVulkanFramebuffer>();
    vulkan_framebuffer_init(context_, framebuffer, description);

    RHIVulkanRenderPassRef vulkan_render_pass = static_ref_cast<RHIVulkanRenderPass>(render_pass);

    Array<VkImageView> attachments = description.attachments.map<VkImageView>([](const RHITextureViewHandle& texture_view) {
        SharedRef<RHIVulkanTextureView> vulkan_texture_view = static_ref_cast<RHIVulkanTextureView>(texture_view);
        return vulkan_texture_view->get_handle();
    });

    VkFramebufferCreateInfo framebuffer_create_info = {};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = vulkan_render_pass->get_handle();
    framebuffer_create_info.attachmentCount = description.attachments.size();
    framebuffer_create_info.pAttachments = attachments.data();
    framebuffer_create_info.width = description.width;
    framebuffer_create_info.height = description.height;
    framebuffer_create_info.layers = description.layers;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateFramebuffer(context_.device, &framebuffer_create_info, context_.allocator, &framebuffer->get_handle()));

    return framebuffer;
}

void RHIVulkanDevice::destroy_framebuffer(RHIFramebufferHandle framebuffer) {
    LCHECK(framebuffer);
    SharedRef<RHIVulkanFramebuffer> vulkan_framebuffer = static_ref_cast<RHIVulkanFramebuffer>(framebuffer);
    vulkan_framebuffer_destroy(context_, vulkan_framebuffer);
}

RHISemaphoreHandle RHIVulkanDevice::create_semaphore() {
    SharedRef<RHIVulkanSemaphore> semaphore = new_ref<RHIVulkanSemaphore>();

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateSemaphore(context_.device, &semaphore_create_info, context_.allocator, &semaphore->get_handle()));

    return semaphore;
}

void RHIVulkanDevice::destroy_semaphore(RHISemaphoreHandle semaphore) {
    LCHECK(semaphore);

    SharedRef<RHIVulkanSemaphore> vk_semaphore = static_ref_cast<RHIVulkanSemaphore>(semaphore);
    VulkanAPI::lvkDestroySemaphore(context_.device, vk_semaphore->get_handle(), context_.allocator);
}

RHIFenceHandle RHIVulkanDevice::create_fence() {
    VkFence fence;
    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = VK_NULL_HANDLE;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateFence(context_.device, &fence_create_info, context_.allocator, &fence));

    return new_ref<RHIVulkanFence>(fence, true);
}

void RHIVulkanDevice::destroy_fence(RHIFenceHandle fence) {
    LCHECK(fence);

    SharedRef<RHIVulkanFence> vk_fence = static_ref_cast<RHIVulkanFence>(fence);
    VulkanAPI::lvkDestroyFence(context_.device, vk_fence->get_handle(), context_.allocator);
}

Array<RHICommandQueueRef> RHIVulkanDevice::get_command_queues() {
    return context_.command_queues;
}

RHIVulkanDevice::RHIVulkanDevice(VulkanContext& context)
    : context_(context) {
}

}  //namespace licht