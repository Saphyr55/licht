#pragma once

#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

class RHIVulkanDevice : public RHIDevice {
public:
    virtual void wait_for_fences(const Array<RHIFenceHandle>& fences) override;
    virtual void reset_fences(const Array<RHIFenceHandle>& fences) override;
    virtual void wait_idle() override;

    virtual RHICommandAllocatorRef create_command_allocator(uint32 count) override;
    virtual void destroy_command_allocator(RHICommandAllocatorRef command_allocator) override;

    virtual RHITextureHandle create_texture(const RHITextureDescription& description) override;
    virtual void destroy_texture(RHITextureHandle texture) override;

    virtual RHITextureViewHandle create_texture_view(const RHITextureViewDescription& description) override;
    virtual void destroy_texture_view(RHITextureViewHandle texture_view) override;

    virtual RHIRenderPassHandle create_render_pass(const RHIRenderPassDescription& description) override;
    virtual void destroy_render_pass(RHIRenderPassHandle render_pass) override;

    virtual RHIPipelineHandle create_graphics_pipeline(const RHIPipelineDescription& description) override;
    virtual void destroy_graphics_pipeline(RHIPipelineHandle pipeline) override;

    virtual RHIBufferHandle create_buffer() override;
    virtual void destroy_buffer(RHIBufferHandle buffer) override; 

    virtual RHISwapchainHandle create_swapchain() override;
    virtual void recreate_swapchain(RHISwapchainHandle swapchain) override;
    virtual void destroy_swapchain(RHISwapchainHandle swapchain) override; 

    virtual RHIFramebufferHandle create_framebuffer(RHIRenderPassHandle render_pass,
                                                    const RHIFramebufferDescription& description) override;
    virtual void destroy_framebuffer(RHIFramebufferHandle framebuffer) override;

    virtual RHISemaphoreHandle create_semaphore() override;
    virtual void destroy_semaphore(RHISemaphoreHandle semaphore) override;

    virtual RHIFenceHandle create_fence() override;
    virtual void destroy_fence(RHIFenceHandle fence) override;

    virtual RHICommandQueueRef query_queue(RHIQueueType type) override;

public:
    RHIVulkanDevice(VulkanContext& context);

private:
    RHICommandQueueRef present_queue_;
    RHICommandQueueRef graphics_queue_;
    VulkanContext& context_; 
};

}  //namespace licht
