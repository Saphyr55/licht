#pragma once

#include <vulkan/vulkan_core.h>
#include "licht/core/collection/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/rhi.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

class RHIVulkanCommandBuffer : public RHICommandBuffer {
public:
    virtual void begin() override;

    virtual void end() override;

    virtual void begin_render_pass(const RHIRenderPassBeginInfo& render_pass_begin_info) override;

    virtual void end_render_pass() override;

    virtual void bind_pipeline(RHIPipelineHandle pipeline) override;

    virtual void set_scissors(const Rect2D* scissors, uint32 count) override;

    virtual void set_viewports(const Viewport* viewports, uint32 count) override;

    virtual void draw(const RHIDrawCommand& command) override;

    VkCommandBuffer get_handle();

public:
    RHIVulkanCommandBuffer(VkCommandBuffer command_buffer)
        : command_buffer_(command_buffer) {
    }
    ~RHIVulkanCommandBuffer() = default;

private:
    VkCommandBuffer command_buffer_;
};

using RHIVulkanCommandBufferRef = SharedRef<RHIVulkanCommandBuffer>;

class RHIVulkanCommandAllocator : public RHICommandAllocator {
public:
    virtual RHICommandBufferHandle open(uint32 index) override;

    virtual void reset_command_buffer(RHICommandBufferHandle command_buffer) override;

public:
    const VkCommandPool get_command_pool() const;

    const Array<VkCommandBuffer>& get_command_buffers() const;

    void initialize_command_pool();

    void allocate_command_buffers();

    void destroy();

public:
    RHIVulkanCommandAllocator(VulkanContext& context, uint32 count);
    ~RHIVulkanCommandAllocator() = default;
    
private:
    VkCommandPool command_pool_;
    Array<VkCommandBuffer> command_buffers_;
    uint32 count_;

    VulkanContext& context_;
};

}  //namespace licht