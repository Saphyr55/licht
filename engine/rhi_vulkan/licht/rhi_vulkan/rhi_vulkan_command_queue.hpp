#pragma once

#include "licht/core/function/function_ref.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanCommandQueue : public RHICommandQueue {
public:
    virtual void submit(const Array<RHICommandBufferHandle>& command_buffers, RHIFrameContext& context) override;
        
    virtual void present(RHISwapchainHandle swapchain, RHIFrameContext& context) override;

    virtual void wait_idle() override;

    virtual RHIQueueType get_type() const override {
        return type_;
    }

    VkQueue& get_handle() {
        return queue_;
    }

    virtual bool is_present_mode() override;

public:
    RHIVulkanCommandQueue(VulkanContext& context, VkQueue queue, RHIQueueType type, bool is_present_mode_supported);

private:
    VulkanContext& context_;
    VkQueue queue_;
    RHIQueueType type_;
    bool is_present_mode_supported_;
};


}  //namespace licht