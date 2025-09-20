#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/fence.hpp"
#include "licht/rhi/semaphore.hpp"
#include "licht/rhi/swapchain.hpp"

namespace licht {

/**
 * @brief Types of hardware queues available in the RHI.
 */
enum class RHIQueueType {
    Unknown,   ///< Unknown or uninitialized queue type.
    Graphics,  ///< Graphics queue, capable of rendering and presentation.
    Compute,   ///< Compute queue.
    Transfer,  ///< Transfer queue.
};

/**
 * @brief Abstract interface for a GPU command queue.
 *
 * A command queue is responsible for submitting command buffers to the GPU,
 * synchronizing via fences/semaphores, and presenting images to a swapchain.
 */
class RHICommandQueue {
public:
    /**
     * @brief Submit command buffers to this queue.
     * @param command_buffers Array of command buffer handles to submit.
     * @param context Frame context used for synchronization and per-frame resources.
     */
    virtual void submit(const Array<RHICommandBufferHandle>& command_buffers, RHIFrameContext& context) = 0;

    /**
     * @brief Present the current image to a swapchain.
     * @param swapchain Swapchain handle to present into.
     * @param context Frame context used for synchronization and per-frame resources.
     */
    virtual void present(RHISwapchainHandle swapchain, RHIFrameContext& context) = 0;

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
     * @brief Destructor.
     */
    virtual ~RHICommandQueue() = default;
};

/**
 * @brief Shared reference type for RHICommandQueue.
 */
using RHICommandQueueRef = SharedRef<RHICommandQueue>;

}  // namespace licht
