#pragma once

#include "licht/core/memory/heap_allocator.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/fence.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/sampler.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

class VulkanDevice : public RHIDevice {
public:
    virtual void wait_idle() override;

    virtual void wait_fence(RHIFence* fence) override;
    virtual void reset_fence(RHIFence* fence) override;

    virtual RHIBufferPoolRef create_buffer_pool() override;
    virtual RHITexturePoolRef create_texture_pool() override;

    virtual RHIShaderResourceGroupLayout* create_shader_resource_layout(const Array<RHIShaderResourceBinding>& bindings) override;
    virtual void destroy_shader_resource_layout(RHIShaderResourceGroupLayout* layout) override;

    virtual RHIShaderResourceGroupPool* create_shader_resource_pool(size_t max_sets,
                                                                    const Array<RHIShaderResourceBinding>& total_bindings) override;
    virtual void destroy_shader_resource_pool(RHIShaderResourceGroupPool* descriptor_set_layout) override;

    virtual RHICommandAllocator* create_command_allocator(const RHICommandAllocatorDescription& description) override;
    virtual void destroy_command_allocator(RHICommandAllocator* command_allocator) override;

    virtual RHITextureView* create_texture_view(const RHITextureViewDescription& description) override;
    virtual void destroy_texture_view(RHITextureView* texture_view) override;

    virtual RHISampler* create_sampler(const RHISamplerDescription& description) override;
    virtual void destroy_sampler(RHISampler* sampler) override;

    virtual RHIRenderPass* create_render_pass(const RHIRenderPassDescription& description) override;
    virtual void destroy_render_pass(RHIRenderPass* render_pass) override;

    virtual RHIGraphicsPipeline* create_graphics_pipeline(const RHIGraphicsPipelineDescription& description) override;
    virtual void destroy_graphics_pipeline(RHIGraphicsPipeline* pipeline) override;

    virtual RHISwapchain* create_swapchain(uint32 width, uint32 height, uint32 image_count) override;
    virtual void recreate_swapchain(RHISwapchain* swapchain, uint32 width, uint32 height) override;
    virtual void destroy_swapchain(RHISwapchain* swapchain) override;

    virtual RHIFramebuffer* create_framebuffer(const RHIFramebufferDescription& description) override;
    virtual void destroy_framebuffer(RHIFramebuffer* framebuffer) override;

    virtual RHISemaphore* create_semaphore() override;
    virtual void destroy_semaphore(RHISemaphore* semaphore) override;

    virtual RHIFence* create_fence() override;
    virtual void destroy_fence(RHIFence* fence) override;

    virtual Array<RHICommandQueueRef> get_command_queues() override;

public:
    VulkanDevice();

private:
    HeapAllocator& allocator_;
    VulkanContext& context_;
};

}  //namespace licht
