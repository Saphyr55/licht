#pragma once

#include <vulkan/vulkan_core.h>
#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

class RHIVulkanCommandBuffer : public RHICommandBuffer {
public:
    virtual void begin(RHICommandBufferUsage usage = RHICommandBufferUsage::None) override;

    virtual void end() override;

    virtual void begin_render_pass(const RHIRenderPassBeginInfo& render_pass_begin_info) override;

    virtual void end_render_pass() override;

    virtual void bind_pipeline(RHIPipelineHandle pipeline) override;

    virtual void bind_vertex_buffers(const Array<RHIBufferHandle>& buffers) override;
    virtual void bind_index_buffer(RHIBufferHandle buffer) override;

    virtual void set_scissors(const Rect2D* scissors, uint32 count) override;

    virtual void set_viewports(const Viewport* viewports, uint32 count) override;

    virtual void copy_buffer(RHIBufferHandle source, RHIBufferHandle destination, const RHIBufferCopyCommand& command) override;

    virtual void draw(const RHIDrawCommand& command) override;
    virtual void draw(const RHIDrawIndexedCommand& command) override;

    inline VkCommandBuffer& get_handle() {
        return command_buffer_;
    }

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
    virtual RHICommandBufferHandle open(uint32 index = 0) override;

    virtual void reset_command_buffer(RHICommandBufferHandle command_buffer) override;

public:
    VkCommandPool& get_command_pool();

    const Array<VkCommandBuffer>& get_command_buffers() const;

    void initialize_command_pool();

    void allocate_command_buffers();

    void destroy();

public:
    RHIVulkanCommandAllocator(VulkanContext& context, const RHICommandAllocatorDescription& description);
    ~RHIVulkanCommandAllocator() = default;
    
private:
    VkCommandPool command_pool_;
    Array<VkCommandBuffer> command_buffers_;
    RHIQueueType queue_type_;
    uint32 queue_family_index_ = 0;
    uint32 count_ = 0;
    
    VulkanContext& context_;
};

}  //namespace licht