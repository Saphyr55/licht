#pragma once

#include <vulkan/vulkan_core.h>
#include "licht/core/collection/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/rhi.hpp"

namespace licht {

class VulkanContext;

class RHIVulkanCommandBuffer : public RHICommandBuffer {
public:
    virtual void begin() override;

    virtual void end() override;

    virtual void set_scissors(const Rect2D* scissors, uint32 count) override;

    virtual void set_viewports(const Viewport* viewports, uint32 count) override;

    virtual void draw(const RHIDrawCommand& command) override;
};

class VulkanCommandBufferRegistry {
public:
    const VkCommandPool get_command_pool() const;

    const Array<VkCommandBuffer>& get_command_buffers() const;

public:
    VulkanCommandBufferRegistry(VulkanContext& context, uint32 frame_count);

private:
    VkCommandPool command_pool_;
    Array<VkCommandBuffer> command_buffers_;

    VulkanContext& context_;
};

}  //namespace licht