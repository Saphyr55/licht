#include "licht/rhi/texture.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/device.hpp"

namespace licht {

const RHITextureDescription& RHITexture::get_description() const {
    return description_;
}

void rhi_transition_texture(RHIDeviceRef device, const RHITextureBarrier& barrier, const RHICommandQueueRef& queue) {
    RHICommandAllocatorDescription transfer_command_allocator_desc = {};
    transfer_command_allocator_desc.command_queue = queue;

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
