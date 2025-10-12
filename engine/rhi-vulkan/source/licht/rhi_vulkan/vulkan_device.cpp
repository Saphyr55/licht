#include "licht/rhi_vulkan/vulkan_device.hpp"
#include "licht/core/memory/default_allocator.hpp"
#include "licht/core/memory/deleter.hpp"
#include "licht/core/memory/memory.hpp"

#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/fence.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/semaphore.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/vulkan_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_buffer_pool.hpp"
#include "licht/rhi_vulkan/vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_description_set.hpp"
#include "licht/rhi_vulkan/vulkan_framebuffer.hpp"
#include "licht/rhi_vulkan/vulkan_graphics_pipeline.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_sampler.hpp"
#include "licht/rhi_vulkan/vulkan_swapchain.hpp"
#include "licht/rhi_vulkan/vulkan_sync.hpp"
#include "licht/rhi_vulkan/vulkan_texture.hpp"
#include "licht/rhi_vulkan/vulkan_texture_view.hpp"

#include <vulkan/vulkan_core.h>
#include <type_traits>
#include <utility>
#include "vulkan_device.hpp"

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

RHIShaderResourceGroupLayout* VulkanDevice::create_shader_resource_layout(const Array<RHIShaderResourceBinding>& bindings) {
    VulkanShaderResourceGroupLayout* vk_descriptor_layout = lnew(allocator_, VulkanShaderResourceGroupLayout());
    vk_descriptor_layout->initialize(bindings);
    return vk_descriptor_layout;
}

void VulkanDevice::destroy_shader_resource_layout(RHIShaderResourceGroupLayout* layout) {
    LCHECK(layout)
    VulkanShaderResourceGroupLayout* vklayout = static_cast<VulkanShaderResourceGroupLayout*>(layout);
    vklayout->destroy();
    ldelete(allocator_, vklayout);
}

RHIShaderResourceGroupPool* VulkanDevice::create_shader_resource_pool(size_t max_sets, const Array<RHIShaderResourceBinding>& total_bindings) {
    VulkanShaderResourceGroupPool* vk_descriptor_pool = lnew(allocator_, VulkanShaderResourceGroupPool());
    vk_descriptor_pool->initialize(max_sets, total_bindings);
    return vk_descriptor_pool;
}

void VulkanDevice::destroy_shader_resource_pool(RHIShaderResourceGroupPool* desc) {
    LCHECK(desc)

    VulkanShaderResourceGroupPool* vkdesc = static_cast<VulkanShaderResourceGroupPool*>(desc);
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

RHIBufferPoolRef VulkanDevice::create_buffer_pool() {
    auto deleter = Deleter<VulkanBufferPool, decltype(allocator_)>(allocator_);
    return SharedRef<VulkanBufferPool>(lnew(allocator_, VulkanBufferPool()), deleter);
}

RHITexture* VulkanDevice::create_texture(const RHITextureDescription& description) {
    VulkanTexture* vk_texture = lnew(allocator_, VulkanTexture());
    vk_texture->initialize(description);
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
    texture_view->initialize(description);
    return texture_view;
}

void VulkanDevice::destroy_texture_view(RHITextureView* texture_view) {
    LCHECK(texture_view)

    VulkanTextureView* vk_texture_view = static_cast<VulkanTextureView*>(texture_view);
    VulkanAPI::lvkDestroyImageView(context_.device, vk_texture_view->get_handle(), context_.allocator);

    ldelete(allocator_, vk_texture_view);
}

RHISampler* VulkanDevice::create_sampler(const RHISamplerDescription& description) {
    VulkanSampler* sampler = lnew(allocator_, VulkanSampler());
    sampler->initialize(description);
    return sampler;
}

void VulkanDevice::destroy_sampler(RHISampler* sampler) {
    LCHECK(sampler)

    VulkanSampler* vksampler = static_cast<VulkanSampler*>(sampler);
    vksampler->destroy();

    ldelete(allocator_, vksampler);
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

RHIGraphicsPipeline* VulkanDevice::create_graphics_pipeline(const RHIGraphicsPipelineDescription& description) {
    VulkanGraphicsPipeline* graphics_pipeline = lnew(allocator_, VulkanGraphicsPipeline());
    graphics_pipeline->initialize(description);
    return graphics_pipeline;
}

void VulkanDevice::destroy_graphics_pipeline(RHIGraphicsPipeline* pipeline) {
    VulkanGraphicsPipeline* graphics_pipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
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