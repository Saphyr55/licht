#include "licht/rhi/texture.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/device.hpp"

namespace licht {

const RHITextureDescription& RHITexture::get_description() const {
    return description_;
}

void rhi_transition_texture(SharedRef<RHIDevice> device, const RHITextureLayoutTransition& barrier, const RHICommandQueueRef& queue) {
    RHICommandAllocatorDescription transfer_command_allocator_desc = {};
    transfer_command_allocator_desc.command_queue = queue;

    RHICommandAllocator* cmd_allocator = device->create_command_allocator(transfer_command_allocator_desc);

    // Open a transfer command buffer
    RHICommandBuffer* cmd_buffer = cmd_allocator->open();
    cmd_allocator->reset_command_buffer(cmd_buffer);

    cmd_buffer->begin();
    {
        cmd_buffer->transition_texture_layout(barrier);
    }
    cmd_buffer->end();

    device->destroy_command_allocator(cmd_allocator);
}

}  //namespace licht
