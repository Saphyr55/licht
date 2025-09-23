#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/descriptor_set.hpp"
#include "licht/rhi/fence.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/pipeline/pipeline.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/semaphore.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/texture.hpp"

namespace licht {

class RHIDevice {
public:
    virtual void wait_idle() = 0;

    virtual void wait_fence(RHIFenceHandle fence) = 0;
    virtual void reset_fence(RHIFenceHandle fence) = 0;

    virtual RHIDescriptorPoolRef create_descriptor_pool(RHIPipelineHandle pipeline, 
                                                        const RHIDescriptorSetInformation& information) = 0;
    virtual void destroy_descriptor_pool(RHIDescriptorPoolRef descriptor_set_layout) = 0;

    virtual RHICommandAllocatorRef create_command_allocator(const RHICommandAllocatorDescription& description) = 0;
    virtual void destroy_command_allocator(RHICommandAllocatorRef command_allocator) = 0;

    virtual RHITextureHandle create_texture(const RHITextureDescription& description) = 0;
    virtual void destroy_texture(RHITextureHandle texture) = 0;

    virtual RHITextureViewHandle create_texture_view(const RHITextureViewDescription& description) = 0;
    virtual void destroy_texture_view(RHITextureViewHandle texture_view) = 0;

    virtual RHIRenderPassHandle create_render_pass(const RHIRenderPassDescription& description) = 0;
    virtual void destroy_render_pass(RHIRenderPassHandle render_pass) = 0;

    virtual RHIPipelineHandle create_graphics_pipeline(const RHIPipelineDescription& description) = 0;
    virtual void destroy_graphics_pipeline(RHIPipelineHandle pipeline) = 0;

    virtual RHIBufferHandle create_buffer(RHIBufferDescription description) = 0;
    virtual void destroy_buffer(RHIBufferHandle buffer) = 0;

    virtual RHISwapchainHandle create_swapchain(uint32 width, uint32 height) = 0;
    virtual void recreate_swapchain(RHISwapchainHandle swapchain, uint32 width, uint32 height) = 0;
    virtual void destroy_swapchain(RHISwapchainHandle swapchain) = 0;

    virtual RHIFramebufferHandle create_framebuffer(RHIRenderPassHandle render_pass,
                                                    const RHIFramebufferDescription& description) = 0;
    virtual void destroy_framebuffer(RHIFramebufferHandle framebuffer) = 0;

    virtual RHISemaphoreHandle create_semaphore() = 0;
    virtual void destroy_semaphore(RHISemaphoreHandle semaphore) = 0;

    virtual RHIFenceHandle create_fence() = 0;
    virtual void destroy_fence(RHIFenceHandle fence) = 0;

    virtual Array<RHICommandQueueRef> get_command_queues() = 0;
};

using RHIDeviceHandle = SharedRef<RHIDevice>;

}  //namespace licht