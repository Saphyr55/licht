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
    virtual void submit(const Array<RHICommandBufferHandle>& command_buffers, RHIFrameContext& context) = 0;

    virtual void present(RHISwapchainHandle swapchain, RHIFrameContext& context) = 0;

    virtual RHIQueueType get_type() const = 0;

    virtual void wait_idle() = 0;

    virtual ~RHICommandQueue() = default;
};

using RHICommandQueueRef = SharedRef<RHICommandQueue>;

}  //namespace licht