#pragma once

#include "licht/core/defines.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/pipeline/pipeline.hpp"
#include "licht/rhi/rhi_fowards.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi/render_pass.hpp"

namespace licht {

class RHIDevice {
public:
    virtual void wait_idle() = 0;

    virtual void wait_fence(RHIFenceRef fence) = 0;
    virtual void reset_fence(RHIFenceRef fence) = 0;

    virtual RHIDescriptorPoolRef create_descriptor_pool(RHIPipelineRef pipeline, 
                                                        const RHIDescriptorSetInformation& information) = 0;
    virtual void destroy_descriptor_pool(RHIDescriptorPoolRef descriptor_set_layout) = 0;

    virtual RHICommandAllocatorRef create_command_allocator(const RHICommandAllocatorDescription& description) = 0;
    virtual void destroy_command_allocator(RHICommandAllocatorRef command_allocator) = 0;

    virtual RHITextureRef create_texture(const RHITextureDescription& description) = 0;
    virtual void destroy_texture(RHITextureRef texture) = 0;

    virtual RHITextureViewRef create_texture_view(const RHITextureViewDescription& description) = 0;
    virtual void destroy_texture_view(RHITextureViewRef texture_view) = 0;

    virtual RHIRenderPassRef create_render_pass(const RHIRenderPassDescription& description) = 0;
    virtual void destroy_render_pass(RHIRenderPassRef render_pass) = 0;

    virtual RHIPipelineRef create_graphics_pipeline(const RHIPipelineDescription& description) = 0;
    virtual void destroy_graphics_pipeline(RHIPipelineRef pipeline) = 0;

    virtual RHIBufferRef create_buffer(RHIBufferDescription description) = 0;
    virtual void destroy_buffer(RHIBufferRef buffer) = 0;

    virtual RHISwapchainRef create_swapchain(uint32 width, uint32 height, uint32 image_count) = 0;
    virtual void recreate_swapchain(RHISwapchainRef swapchain, uint32 width, uint32 height) = 0;
    virtual void destroy_swapchain(RHISwapchainRef swapchain) = 0;

    virtual RHIFramebufferRef create_framebuffer(RHIRenderPassRef render_pass,
                                                    const RHIFramebufferDescription& description) = 0;
    virtual void destroy_framebuffer(RHIFramebufferRef framebuffer) = 0;

    virtual RHISemaphoreRef create_semaphore() = 0;
    virtual void destroy_semaphore(RHISemaphoreRef semaphore) = 0;

    virtual RHIFenceRef create_fence() = 0;
    virtual void destroy_fence(RHIFenceRef fence) = 0;

    virtual Array<RHICommandQueueRef> get_command_queues() = 0;
};

}  //namespace licht