#include "licht/core/memory/default_allocator.hpp"
#include "licht/core/memory/memory.hpp"
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
#include "licht/rhi_vulkan/vulkan_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_command_queue.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_description_set.hpp"
#include "licht/rhi_vulkan/vulkan_framebuffer.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_pipeline.hpp"
#include "licht/rhi_vulkan/vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_swapchain.hpp"
#include "licht/rhi_vulkan/vulkan_sync.hpp"
#include "licht/rhi_vulkan/vulkan_texture.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

void VulkanDevice::wait_fence(RHIFence* fence) {
    if (fence->is_signaled()) {
        return;
    }

    RHIVulkanFence* rhi_vk_fence = static_cast<RHIVulkanFence*>(fence);

    VkResult result_wait = VulkanAPI::lvkWaitForFences(context_.device, 1, &rhi_vk_fence->get_handle(), VK_TRUE, UINT64_MAX);
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

void VulkanDevice::reset_fence(RHIFence* fence) {
    if (!fence->is_signaled()) {
        return;
    }

    RHIVulkanFence* rhi_vk_fence = static_cast<RHIVulkanFence*>(fence);
    LICHT_VULKAN_CHECK(VulkanAPI::lvkResetFences(context_.device, 1, &rhi_vk_fence->get_handle()));

    rhi_vk_fence->set_signaled(false);
}

RHIShaderResourcePool* VulkanDevice::create_shader_resource_pool(RHIPipeline* pipeline, size_t count) {
    VulkanPipeline* vkpileline = static_cast<VulkanPipeline*>(pipeline);
    VulkanDescriptorPool* vk_descriptor_pool = lnew(allocator_, VulkanDescriptorPool(context_, vkpileline->get_descriptor_set_layout(), count));
    vk_descriptor_pool->initialize();
    return vk_descriptor_pool;
}

void VulkanDevice::destroy_shader_resource_pool(RHIShaderResourcePool* desc) {
    LCHECK(desc)
    
    VulkanDescriptorPool* vkdesc = static_cast<VulkanDescriptorPool*>(desc);
    vkdesc->destroy();
    ldelete(allocator_, vkdesc);
}

void VulkanDevice::wait_idle() {
    LICHT_VULKAN_CHECK(VulkanAPI::lvkDeviceWaitIdle(context_.device));
}

RHICommandAllocator* VulkanDevice::create_command_allocator(const RHICommandAllocatorDescription& description) {
    RHIVulkanCommandAllocator* vulkan_command_allocator = lnew(allocator_, RHIVulkanCommandAllocator(context_, description));
    vulkan_command_allocator->initialize_command_pool();
    vulkan_command_allocator->allocate_command_buffers();
    return vulkan_command_allocator;
}

void VulkanDevice::destroy_command_allocator(RHICommandAllocator* command_allocator) {
    LCHECK(command_allocator)

    RHIVulkanCommandAllocator* vulkan_command_allocator = static_cast<RHIVulkanCommandAllocator*>(command_allocator);
    vulkan_command_allocator->destroy();
    ldelete(allocator_, vulkan_command_allocator);
}

RHIBuffer* VulkanDevice::create_buffer(RHIBufferDescription description) {
    VulkanBuffer* buffer = lnew(allocator_, VulkanBuffer(context_, description));
    buffer->initialize();
    return buffer;
}

void VulkanDevice::destroy_buffer(RHIBuffer* buffer) {
    LCHECK(buffer)

    VulkanBuffer* vkbuffer = static_cast<VulkanBuffer*>(buffer);
    vkbuffer->destroy();

    ldelete(allocator_, vkbuffer);
}

RHITexture* VulkanDevice::create_texture(const RHITextureDescription& description) {
    VulkanTexture* vk_texture = lnew(allocator_, VulkanTexture(
        context_, 
        description,
        VK_IMAGE_TYPE_2D, 
        {}));

    vk_texture->initialize();
    return vk_texture;
}

void VulkanDevice::destroy_texture(RHITexture* texture) {
    LCHECK(texture)

    VulkanTexture* vktexture = static_cast<VulkanTexture*>(texture);
    vktexture->destroy();
    
    ldelete(allocator_, vktexture);
}

RHITextureView* VulkanDevice::create_texture_view(const RHITextureViewDescription& description) {
    VulkanTextureView* texture_view = lnew(allocator_, VulkanTextureView());

    VulkanTexture* texture = static_cast<VulkanTexture*>(description.texture);

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

void VulkanDevice::destroy_texture_view(RHITextureView* texture_view) {
    LCHECK(texture_view)

    VulkanTextureView* vk_texture_view = static_cast<VulkanTextureView*>(texture_view);
    VulkanAPI::lvkDestroyImageView(context_.device, vk_texture_view->get_handle(), context_.allocator);

    ldelete(allocator_, vk_texture_view);
}

RHIRenderPass* VulkanDevice::create_render_pass(const RHIRenderPassDescription& description) {
    VulkanRenderPass* render_pass = lnew(allocator_, VulkanRenderPass(context_, description));
    render_pass->initialize();
    return render_pass;
}

void VulkanDevice::destroy_render_pass(RHIRenderPass* render_pass) {
    LCHECK(render_pass)

    VulkanRenderPass* vulkan_render_pass = static_cast<VulkanRenderPass*>(render_pass);
    vulkan_render_pass->destroy();

    ldelete(allocator_, vulkan_render_pass);
}

RHIPipeline* VulkanDevice::create_graphics_pipeline(const RHIPipelineDescription& description) {
    VulkanPipeline* graphics_pipeline = lnew(allocator_, VulkanPipeline(context_, description));
    graphics_pipeline->initialize();
    return graphics_pipeline;
}

void VulkanDevice::destroy_graphics_pipeline(RHIPipeline* pipeline) {
    VulkanPipeline* graphics_pipeline = static_cast<VulkanPipeline*>(pipeline);
    graphics_pipeline->destroy();
    ldelete(allocator_, graphics_pipeline);
}

RHISwapchain* VulkanDevice::create_swapchain(uint32 width, uint32 height, uint32 image_count) {
    VulkanSwapchain* swapchain = lnew(allocator_, VulkanSwapchain(context_, width, height, image_count));
    swapchain->initialize();
    return swapchain;
}

void VulkanDevice::recreate_swapchain(RHISwapchain* swapchain, uint32 width, uint32 height) {
    VulkanSwapchain* vulkan_swapchain = static_cast<VulkanSwapchain*>(swapchain);
    vulkan_swapchain->set_extent({width, height});
    vulkan_swapchain->destroy();
    vulkan_swapchain->initialize();
}

void VulkanDevice::destroy_swapchain(RHISwapchain* swapchain) {
    LCHECK(swapchain);

    VulkanSwapchain* vulkan_swapchain = static_cast<VulkanSwapchain*>(swapchain);
    vulkan_swapchain->destroy();

    ldelete(allocator_, vulkan_swapchain);
}

RHIFramebuffer* VulkanDevice::create_framebuffer(const RHIFramebufferDescription& description) {
    VulkanFramebuffer* framebuffer = lnew(allocator_, VulkanFramebuffer(context_, description));
    framebuffer->initialize();
    return framebuffer;
}

void VulkanDevice::destroy_framebuffer(RHIFramebuffer* framebuffer) {
    LCHECK(framebuffer);

    VulkanFramebuffer* vulkan_framebuffer = static_cast<VulkanFramebuffer*>(framebuffer);
    vulkan_framebuffer->destroy();
    ldelete(allocator_, vulkan_framebuffer);
}

RHISemaphore* VulkanDevice::create_semaphore() {
    RHIVulkanSemaphore* semaphore = lnew(allocator_, RHIVulkanSemaphore());

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateSemaphore(context_.device, &semaphore_create_info, context_.allocator, &semaphore->get_handle()));

    return semaphore;
}

void VulkanDevice::destroy_semaphore(RHISemaphore* semaphore) {
    LCHECK(semaphore);

    RHIVulkanSemaphore* vk_semaphore = static_cast<RHIVulkanSemaphore*>(semaphore);
    VulkanAPI::lvkDestroySemaphore(context_.device, vk_semaphore->get_handle(), context_.allocator);

    ldelete(allocator_, vk_semaphore);
}

RHIFence* VulkanDevice::create_fence() {
    VkFence fence;
    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = VK_NULL_HANDLE;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateFence(context_.device, &fence_create_info, context_.allocator, &fence));

    return lnew(allocator_, RHIVulkanFence(fence, true));
}

void VulkanDevice::destroy_fence(RHIFence* fence) {
    LCHECK(fence);

    RHIVulkanFence* vk_fence = static_cast<RHIVulkanFence*>(fence);
    VulkanAPI::lvkDestroyFence(context_.device, vk_fence->get_handle(), context_.allocator);

    ldelete(allocator_, vk_fence);
}

Array<RHICommandQueueRef> VulkanDevice::get_command_queues() {
    return context_.command_queues;
}

VulkanDevice::VulkanDevice()
    : context_(vulkan_context_get())
    , allocator_(DefaultAllocator::get_instance()) {
}

}  //namespace licht