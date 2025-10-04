#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/fence.hpp"
#include "licht/rhi/semaphore.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

/**
 * @brief Abstract interface for a GPU command queue.
 *
 * A command queue is responsible for submitting command buffers to the GPU,
 * synchronizing via fences/semaphores, and presenting images to a swapchain.
 */
class RHICommandQueue {
public:
    virtual void submit(const Array<RHICommandBufferRef>& command_buffers,
                        const Array<RHISemaphoreRef>& wait_semaphores,
                        const Array<RHISemaphoreRef>& signal_semaphores,
                        const RHIFenceRef fence) = 0;

    /**
     * @brief Present the current image to a swapchain.
     * @param swapchain Swapchain handle to present into.
     * @param context Frame context used for synchronization and per-frame resources.
     */
    virtual void present(RHISwapchainRef swapchain, RHIFrameContext& context) = 0;

    /**
     * @brief Get the type of this queue.
     * @return The queue type.
     */
    virtual RHIQueueType get_type() const = 0;

    /**
     * @brief Block until all submitted work on this queue has completed.
     */
    virtual void wait_idle() = 0;
        
    /**
     * @brief Check if the queue supports the present mode.
     * @return Return true if supported, false otherwise.
     */
    virtual bool is_present_mode() = 0;

    /**
     * @brief Destructor.
     */
    virtual ~RHICommandQueue() = default;
};

}  // namespace licht
