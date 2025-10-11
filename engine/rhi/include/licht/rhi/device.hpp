#pragma once

#include "licht/core/defines.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/pipeline.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi/sampler.hpp"
#include "licht/rhi/texture_view.hpp"
#include "licht/rhi/render_pass.hpp"

namespace licht {

class RHIDevice {
public:
    virtual void wait_idle() = 0;

    virtual void wait_fence(RHIFence* fence) = 0;
    virtual void reset_fence(RHIFence* fence) = 0;

    virtual RHIShaderResourceGroupLayout* create_shader_resource_layout(const Array<RHIShaderResourceBinding>& bindings) = 0;
    virtual void destroy_shader_resource_layout(RHIShaderResourceGroupLayout* layout) = 0;

    virtual RHIShaderResourceGroupPool* create_shader_resource_pool(size_t max_sets, const Array<RHIShaderResourceBinding>& total_bindings) = 0;
    virtual void destroy_shader_resource_pool(RHIShaderResourceGroupPool* group_pool) = 0;

    virtual RHICommandAllocator* create_command_allocator(const RHICommandAllocatorDescription& description) = 0;
    virtual void destroy_command_allocator(RHICommandAllocator* command_allocator) = 0;

    virtual RHITexture* create_texture(const RHITextureDescription& description) = 0;
    virtual void destroy_texture(RHITexture* texture) = 0;

    virtual RHITextureView* create_texture_view(const RHITextureViewDescription& description) = 0;
    virtual void destroy_texture_view(RHITextureView* texture_view) = 0;

    virtual RHISampler* create_sampler(const RHISamplerDescription& description) = 0;
    virtual void destroy_sampler(RHISampler* sampler) = 0;

    virtual RHIRenderPass* create_render_pass(const RHIRenderPassDescription& description) = 0;
    virtual void destroy_render_pass(RHIRenderPass* render_pass) = 0;

    virtual RHIGraphicsPipeline* create_graphics_pipeline(const RHIGraphicsPipelineDescription& description) = 0;
    virtual void destroy_graphics_pipeline(RHIGraphicsPipeline* pipeline) = 0;

    virtual RHIBuffer* create_buffer(RHIBufferDescription description) = 0;
    virtual void destroy_buffer(RHIBuffer* buffer) = 0;

    virtual RHISwapchain* create_swapchain(uint32 width, uint32 height, uint32 image_count) = 0;
    virtual void recreate_swapchain(RHISwapchain* swapchain, uint32 width, uint32 height) = 0;
    virtual void destroy_swapchain(RHISwapchain* swapchain) = 0;

    virtual RHIFramebuffer* create_framebuffer(const RHIFramebufferDescription& description) = 0;
    virtual void destroy_framebuffer(RHIFramebuffer* framebuffer) = 0;

    virtual RHISemaphore* create_semaphore() = 0;
    virtual void destroy_semaphore(RHISemaphore* semaphore) = 0;

    virtual RHIFence* create_fence() = 0;
    virtual void destroy_fence(RHIFence* fence) = 0;

    virtual Array<RHICommandQueueRef> get_command_queues() = 0;
};

}  //namespace licht