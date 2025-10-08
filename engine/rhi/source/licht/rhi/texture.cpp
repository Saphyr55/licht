#include "licht/rhi/texture.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/device.hpp"

namespace licht {

const RHITextureDescription& RHITexture::get_description() const {
    return description_;
}

void rhi_transition_texture(RHIDeviceRef device, const RHITextureBarrier& barrier) {
    // Fetch transfer queue.
    const Array<RHICommandQueueRef>& command_queues = device->get_command_queues();
    RHICommandQueueRef* transfer_queue_ptr = command_queues.get_if([](RHICommandQueueRef queue) {
        return queue->is_transfer_type();
    });

    LCHECK_MSG(transfer_queue_ptr, "Found no transfer command queue.");
    RHICommandQueueRef transfer_queue = *transfer_queue_ptr;

    RHICommandAllocatorDescription transfer_command_allocator_desc = {};
    transfer_command_allocator_desc.command_queue = transfer_queue;

    RHICommandAllocator* transfer_command_allocator = device->create_command_allocator(transfer_command_allocator_desc);

    // Open a transfer command buffer
    RHICommandBuffer* transfer_cmd = transfer_command_allocator->open();
    transfer_command_allocator->reset_command_buffer(transfer_cmd);

    transfer_cmd->begin();
    {
        transfer_cmd->transition_texture(barrier);
    }
    transfer_cmd->end();
}

}  //namespace licht
