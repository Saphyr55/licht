#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/rhi_fowards.hpp"

namespace licht {

RHIBufferDescription RHIDeviceMemoryUploader::create_staging_buffer_description(const RHIStagingBufferContext& context) {
    RHIBufferDescription staging_buffer_description = {};
    staging_buffer_description.access_mode = RHIAccessMode::Shared;
    staging_buffer_description.usage = RHIBufferUsage::TransferSrc;
    staging_buffer_description.memory_usage = RHIBufferMemoryUsage::Host;
    staging_buffer_description.size = context.size;
    return staging_buffer_description;
}

RHIBufferDescription RHIDeviceMemoryUploader::create_buffer_description(const RHIStagingBufferContext& context) {
    RHIBufferDescription buffer_description = {};
    buffer_description.access_mode = RHIAccessMode::Shared;
    buffer_description.usage = context.usage | RHIBufferUsage::TransferDst;
    buffer_description.memory_usage = RHIBufferMemoryUsage::Device;
    buffer_description.size = context.size;
    return buffer_description;
}

RHITexture* RHIDeviceMemoryUploader::send_texture(const RHIStagingBufferContext& context, const RHITextureDescription& description) {
    RHIBufferDescription staging_buffer_description = create_staging_buffer_description(context);
    RHIBuffer* staging_buffer = device_->create_buffer(staging_buffer_description);

    staging_buffer->update(context.data, context.size);

    RHITexture* texture = device_->create_texture(description);

    texture_entries_.append(TextureEntry(staging_buffer, texture, context.size));

    return texture;
}

RHIBuffer* RHIDeviceMemoryUploader::send_buffer(const RHIStagingBufferContext& context) {
    RHIBufferDescription staging_buffer_description = create_staging_buffer_description(context);
    RHIBuffer* staging_buffer = device_->create_buffer(staging_buffer_description);

    staging_buffer->update(context.data, context.size);

    RHIBufferDescription buffer_description = create_buffer_description(context);
    RHIBuffer* buffer = device_->create_buffer(buffer_description);

    buffer_entries_.append(BufferEntry(staging_buffer, buffer, context.size));

    return buffer;
}

// Upload Data from Standing Buffers to Device Buffers
void RHIDeviceMemoryUploader::upload() {
    // Fetch transfer queue.
    const Array<RHICommandQueueRef>& command_queues = device_->get_command_queues();
    RHICommandQueueRef* transfer_queue_ptr = command_queues.get_if([](RHICommandQueueRef queue) {
        return queue->get_type() == RHIQueueType::Transfer;
    });

    LCHECK_MSG(transfer_queue_ptr, "Found no transfer command queue.");
    RHICommandQueueRef transfer_queue = *transfer_queue_ptr;

    RHICommandAllocatorDescription transfer_command_allocator_desc = {};
    transfer_command_allocator_desc.count = 1;  // One command buffer allocated.
    transfer_command_allocator_desc.command_queue = transfer_queue;

    RHICommandAllocator* transfer_command_allocator_ = device_->create_command_allocator(transfer_command_allocator_desc);

    // Open a transfer command buffer
    RHICommandBuffer* transfer_cmd = transfer_command_allocator_->open();
    transfer_command_allocator_->reset_command_buffer(transfer_cmd);

    transfer_cmd->begin();
    {
        for (auto& [staging_buffer, buffer, size] : buffer_entries_) {
            RHIBufferCopyCommand buffer_copy_command = {};
            buffer_copy_command.size = size;
            transfer_cmd->copy_buffer(staging_buffer, buffer, buffer_copy_command);
        }
    }
    transfer_cmd->end();

    // Submit and wait for transfer to complete
    RHIFence* upload_fence = device_->create_fence();
    device_->reset_fence(upload_fence);
    transfer_queue->submit({transfer_cmd}, {}, {}, upload_fence);

    device_->wait_fence(upload_fence);
    device_->destroy_fence(upload_fence);

    // Standing buffers no longer needed after data upload
    for (BufferEntry& entry : buffer_entries_) {
        device_->destroy_buffer(entry.staging);
    }

    device_->destroy_command_allocator(transfer_command_allocator_);
}

}  //namespace licht