#pragma once

#include <vulkan/vulkan_core.h>
#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/linear_allocator.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

class VulkanCommandBuffer : public RHICommandBuffer {
public:
    virtual void begin(RHICommandBufferUsageFlags usage = RHICommandBufferUsageFlags::None) override;

    virtual void end() override;

    virtual void begin_render_pass(const RHIRenderPassBeginInfo& render_pass_begin_info) override;

    virtual void end_render_pass() override;

    virtual void bind_pipeline(RHIPipeline* pipeline) override;
    
    virtual void bind_descriptor_sets(RHIPipeline* pipeline, const Array<RHIShaderResource*>& descriptor_sets) override;

    virtual void bind_vertex_buffers(const Array<RHIBuffer*>& buffers) override;

    virtual void bind_index_buffer(RHIBuffer* buffer) override;

    virtual void set_scissors(const Rect2D* scissors, uint32 count) override;

    virtual void set_viewports(const Viewport* viewports, uint32 count) override;

    virtual void transition_texture(const RHITextureBarrier& barrier) override;

    virtual void copy_buffer_to_texture(const RHICopyBufferToTextureCommand& command) override;

    virtual void copy_buffer(const RHICopyBufferCommand& command) override;

    virtual void draw(const RHIDrawCommand& command) override;

    virtual void draw(const RHIDrawIndexedCommand& command) override;

    inline VkCommandBuffer& get_handle() {
        return command_buffer_;
    }

public:
    VulkanCommandBuffer(VkCommandBuffer command_buffer)
        : command_buffer_(command_buffer) {
    }
    ~VulkanCommandBuffer() = default;

private:
    VkCommandBuffer command_buffer_;
};

using RHIVulkanCommandBufferRef = SharedRef<VulkanCommandBuffer>;

class RHIVulkanCommandAllocator : public RHICommandAllocator {
public:
    virtual RHICommandBuffer* open(uint32 index = 0) override;

    virtual void reset_command_buffer(RHICommandBuffer* command_buffer) override;

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
    Array<VulkanCommandBuffer*> upper_command_buffers_;
    Array<VkCommandBuffer> command_buffers_;
    RHIQueueType queue_type_;
    uint32 queue_family_index_ = 0;
    uint32 count_ = 0;
    LinearAllocator upper_command_buffer_allocator_;

    VulkanContext& context_;
};

}  //namespace licht