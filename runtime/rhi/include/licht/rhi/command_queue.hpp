#pragma once

#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/fence.hpp"
#include "licht/rhi/semaphore.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

/**
 * @brief Interface for a GPU command queue.
 *
 * A command queue is responsible for submitting command buffers to the GPU,
 * synchronizing via fences/semaphores, and presenting images to a swapchain.
 */
class RHICommandQueue {
public:
    virtual void submit(const Array<RHICommandBuffer*>& command_buffers,
                        const Array<RHISemaphore*>& wait_semaphores,
                        const Array<RHISemaphore*>& signal_semaphores,
                        const RHIFence* fence) const = 0;

    /**
     * @brief Present the current image to a swapchain.
     * @param swapchain Swapchain handle to present into.
     * @param context Frame context used for synchronization and per-frame resources.
     */
    virtual void present(RHISwapchain* swapchain, RHIFrameContext& context) const = 0;

    /**
     * @brief Get the type of this queue.
     * @return The queue type.
     */
    virtual RHIQueueType get_type() const = 0;

    inline bool is_queue_type(RHIQueueType type) const {
        return (get_type() & type) == type;
    }

    inline bool is_graphics_type() const {
        return is_queue_type(RHIQueueType::Graphics);
    }

    inline bool is_compute_type() const {
        return is_queue_type(RHIQueueType::Compute);
    }

    inline bool is_transfer_type() const {
        return is_queue_type(RHIQueueType::Transfer);
    }

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
