#pragma once

#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/fence.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

class RHIVulkanDevice : public RHIDevice {
public:
    virtual void wait_idle() override;
    
    virtual void wait_fence(RHIFenceRef fence) override;
    virtual void reset_fence(RHIFenceRef fence) override;
    
    virtual RHIDescriptorPoolRef create_descriptor_pool(RHIPipelineRef pipeline, const RHIDescriptorSetInformation& information) override;
    virtual void destroy_descriptor_pool(RHIDescriptorPoolRef descriptor_set_layout) override;

    virtual RHICommandAllocatorRef create_command_allocator(const RHICommandAllocatorDescription& description) override;
    virtual void destroy_command_allocator(RHICommandAllocatorRef command_allocator) override;

    virtual RHITextureRef create_texture(const RHITextureDescription& description) override;
    virtual void destroy_texture(RHITextureRef texture) override;
    
    virtual RHITextureViewRef create_texture_view(const RHITextureViewDescription& description) override;
    virtual void destroy_texture_view(RHITextureViewRef texture_view) override;

    virtual RHIRenderPassRef create_render_pass(const RHIRenderPassDescription& description) override;
    virtual void destroy_render_pass(RHIRenderPassRef render_pass) override;

    virtual RHIPipelineRef create_graphics_pipeline(const RHIPipelineDescription& description) override;
    virtual void destroy_graphics_pipeline(RHIPipelineRef pipeline) override;

    virtual RHIBufferRef create_buffer(RHIBufferDescription description) override;
    virtual void destroy_buffer(RHIBufferRef buffer) override; 

    virtual RHISwapchainRef create_swapchain(uint32 width, uint32 height, uint32 image_count) override;
    virtual void recreate_swapchain(RHISwapchainRef swapchain, uint32 width, uint32 height) override;
    virtual void destroy_swapchain(RHISwapchainRef swapchain) override; 

    virtual RHIFramebufferRef create_framebuffer(RHIRenderPassRef render_pass,
                                                    const RHIFramebufferDescription& description) override;
    virtual void destroy_framebuffer(RHIFramebufferRef framebuffer) override;

    virtual RHISemaphoreRef create_semaphore() override;
    virtual void destroy_semaphore(RHISemaphoreRef semaphore) override;

    virtual RHIFenceRef create_fence() override;
    virtual void destroy_fence(RHIFenceRef fence) override;

    virtual Array<RHICommandQueueRef> get_command_queues() override;

public:
    RHIVulkanDevice(VulkanContext& context);

private:
    VulkanContext& context_; 
};

}  //namespace licht
