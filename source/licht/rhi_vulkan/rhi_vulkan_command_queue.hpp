#pragma once

#include "licht/rhi/command_queue.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanCommandQueue : public RHICommandQueue {
public:
    virtual void submit(const Array<RHICommandBufferHandle>& command_buffers,
                        const Array<RHISemaphoreHandle>& wait_semaphores,
                        const Array<RHISemaphoreHandle>& signal_semaphores,
                        RHIFenceHandle fence) override;
        
    virtual void present(RHISwapchainHandle swapchain,
                         const Array<RHISemaphoreHandle>& wait_semaphores,
                         uint32 image_index) override;

    virtual void wait_idle() override;

    virtual RHIQueueType get_type() const override {
        return type_;
    }

public:
    RHIVulkanCommandQueue(VulkanContext& context, VkQueue queue, RHIQueueType type);

private:
    VulkanContext& context_;
    VkQueue queue_;
    RHIQueueType type_;
};

}  //namespace licht