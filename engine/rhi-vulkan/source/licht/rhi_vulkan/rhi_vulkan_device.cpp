#include "licht/rhi_vulkan/vulkan_device.hpp"

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
#include "licht/rhi_vulkan/vulkan_sync.hpp"
#include "licht/rhi_vulkan/vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_command_queue.hpp"
#include "licht/rhi_vulkan/vulkan_framebuffer.hpp"
#include "licht/rhi_vulkan/vulkan_pipeline.hpp"
#include "licht/rhi_vulkan/vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_swapchain.hpp"
#include "licht/rhi_vulkan/vulkan_texture.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_description_set.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

void RHIVulkanDevice::wait_fence(RHIFenceRef fence) {
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

void RHIVulkanDevice::reset_fence(RHIFenceRef fence) {
    if (!fence->is_signaled()) {
        return;
    }

    SharedRef<RHIVulkanFence> rhi_vk_fence = static_ref_cast<RHIVulkanFence>(fence);
    VkFence& vkfence = rhi_vk_fence->get_handle();
    LICHT_VULKAN_CHECK(VulkanAPI::lvkResetFences(context_.device, 1, &vkfence));

    rhi_vk_fence->set_signaled(false);
}

RHIDescriptorPoolRef RHIVulkanDevice::create_descriptor_pool(RHIPipelineRef pipeline, const RHIDescriptorSetInformation& information) {
    RHIVulkanPipelineRef vkpileline = static_ref_cast<RHIVulkanPipeline>(pipeline);
    RHIVulkanDescriptorPoolRef vkdesc = new_ref<RHIVulkanDescriptorPool>(context_, vkpileline->get_descriptor_set_layout(), information);
    vkdesc->initialize();
    return vkdesc;
}

void RHIVulkanDevice::destroy_descriptor_pool(RHIDescriptorPoolRef desc) {
    RHIVulkanDescriptorPoolRef vkdesc = static_ref_cast<RHIVulkanDescriptorPool>(desc);
    vkdesc->destroy();
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

RHITextureRef RHIVulkanDevice::create_texture(const RHITextureDescription& description) {
    return RHIVulkanTexture::create(context_, description, {}, {}, {}, {});
}

void RHIVulkanDevice::destroy_texture(RHITextureRef texture) {
    RHIVulkanTextureRef vktexture = static_ref_cast<RHIVulkanTexture>(texture);
    VulkanAPI::lvkDestroyImage(context_.device, vktexture->get_handle(), context_.allocator);
}

RHIBufferRef RHIVulkanDevice::create_buffer(RHIBufferDescription description) {
    RHIVulkanBufferRef buffer = new_ref<RHIVulkanBuffer>(context_, description);
    buffer->initialize();
    return buffer;
}

void RHIVulkanDevice::destroy_buffer(RHIBufferRef buffer) {
    RHIVulkanBufferRef vkbuffer = static_ref_cast<RHIVulkanBuffer>(buffer);
    vkbuffer->destroy();
}

RHITextureViewRef RHIVulkanDevice::create_texture_view(const RHITextureViewDescription& description) {
    VkImageView image_view;
    RHIVulkanTextureViewRef texture_view = new_ref<RHIVulkanTextureView>(image_view);

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

void RHIVulkanDevice::destroy_texture_view(RHITextureViewRef texture_view) {
    RHIVulkanTextureViewRef vk_texture_view = static_ref_cast<RHIVulkanTextureView>(texture_view);
    VulkanAPI::lvkDestroyImageView(context_.device, vk_texture_view->get_handle(), context_.allocator);
}

RHIRenderPassRef RHIVulkanDevice::create_render_pass(const RHIRenderPassDescription& description) {
    RHIVulkanRenderPassRef render_pass = new_ref<RHIVulkanRenderPass>(context_);
    vulkan_render_pass_init(context_, &render_pass->get_handle(), description);
    return render_pass;
}

void RHIVulkanDevice::destroy_render_pass(RHIRenderPassRef render_pass) {
    RHIVulkanRenderPassRef vulkan_render_pass = static_ref_cast<RHIVulkanRenderPass>(render_pass);
    vulkan_render_pass_destroy(context_, vulkan_render_pass->get_handle());
}

RHIPipelineRef RHIVulkanDevice::create_graphics_pipeline(const RHIPipelineDescription& description) {
    RHIVulkanPipelineRef graphics_pipeline = new_ref<RHIVulkanPipeline>(context_, description);
    graphics_pipeline->initialize();
    return graphics_pipeline;
}

void RHIVulkanDevice::destroy_graphics_pipeline(RHIPipelineRef pipeline) {
    RHIVulkanPipelineRef graphics_pipeline = static_ref_cast<RHIVulkanPipeline>(pipeline);
    graphics_pipeline->destroy();
}

RHISwapchainRef RHIVulkanDevice::create_swapchain(uint32 width, uint32 height, uint32 image_count) {
    RHIVulkanSwapchainRef swapchain = new_ref<RHIVulkanSwapchain>(context_, width, height, image_count);
    swapchain->initialize();
    return swapchain;
}

void RHIVulkanDevice::recreate_swapchain(RHISwapchainRef swapchain, uint32 width, uint32 height) {
    SharedRef<RHIVulkanSwapchain> vulkan_swapchain = static_ref_cast<RHIVulkanSwapchain>(swapchain);
    vulkan_swapchain->set_extent({width, height});
    vulkan_swapchain->destroy();
    vulkan_swapchain->initialize();
}

void RHIVulkanDevice::destroy_swapchain(RHISwapchainRef swapchain) {
    LCHECK(swapchain);
    SharedRef<RHIVulkanSwapchain> vulkan_swapchain = static_ref_cast<RHIVulkanSwapchain>(swapchain);
    vulkan_swapchain->destroy();
}

RHIFramebufferRef RHIVulkanDevice::create_framebuffer(RHIRenderPassRef render_pass,
                                                         const RHIFramebufferDescription& description) {
    LCHECK(render_pass);

    SharedRef<RHIVulkanFramebuffer> framebuffer = new_ref<RHIVulkanFramebuffer>(context_, description);
    framebuffer->initialize();

    return framebuffer;
}

void RHIVulkanDevice::destroy_framebuffer(RHIFramebufferRef framebuffer) {
    LCHECK(framebuffer);
    SharedRef<RHIVulkanFramebuffer> vulkan_framebuffer = static_ref_cast<RHIVulkanFramebuffer>(framebuffer);
    vulkan_framebuffer->destroy();
}

RHISemaphoreRef RHIVulkanDevice::create_semaphore() {
    SharedRef<RHIVulkanSemaphore> semaphore = new_ref<RHIVulkanSemaphore>();

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateSemaphore(context_.device, &semaphore_create_info, context_.allocator, &semaphore->get_handle()));

    return semaphore;
}

void RHIVulkanDevice::destroy_semaphore(RHISemaphoreRef semaphore) {
    LCHECK(semaphore);

    SharedRef<RHIVulkanSemaphore> vk_semaphore = static_ref_cast<RHIVulkanSemaphore>(semaphore);
    VulkanAPI::lvkDestroySemaphore(context_.device, vk_semaphore->get_handle(), context_.allocator);
}

RHIFenceRef RHIVulkanDevice::create_fence() {
    VkFence fence;
    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = VK_NULL_HANDLE;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateFence(context_.device, &fence_create_info, context_.allocator, &fence));

    return new_ref<RHIVulkanFence>(fence, true);
}

void RHIVulkanDevice::destroy_fence(RHIFenceRef fence) {
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