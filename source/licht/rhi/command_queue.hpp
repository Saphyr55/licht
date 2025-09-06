#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/fence.hpp"
#include "licht/rhi/semaphore.hpp"
#include "licht/rhi/swapchain.hpp"

namespace licht {

enum class RHIQueueType {
    Unknown,
    Graphics,
    Compute,
    Transfer,
};

class RHICommandQueue {
public:
    virtual void submit(const Array<RHICommandBufferHandle>& command_buffers,
                        const Array<RHISemaphoreHandle>& wait_semaphores,
                        const Array<RHISemaphoreHandle>& signal_semaphores,
                        RHIFenceHandle fence) = 0;

    virtual void present(RHISwapchainHandle swapchain,
                         const Array<RHISemaphoreHandle>& wait_semaphores,
                         uint32 frame_index) = 0;

    virtual RHIQueueType get_type() const = 0;

    virtual void wait_idle() = 0;

    virtual ~RHICommandQueue() = default;
};

using RHICommandQueueRef = SharedRef<RHICommandQueue>;

}  //namespace licht