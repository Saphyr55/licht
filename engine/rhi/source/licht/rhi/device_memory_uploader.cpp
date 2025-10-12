#include "licht/rhi/device_memory_uploader.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/rhi_forwards.hpp"

namespace licht {

RHIDeviceMemoryUploader::RHIDeviceMemoryUploader(RHIDeviceRef device, RHIBufferPoolRef buffer_pool, size_t capacity)
    : device_(device)
    , buffer_pool_(buffer_pool)
    , buffer_entries_(capacity) {
    staging_buffer_pool_ = device_->create_buffer_pool();
}

RHIBufferDescription RHIDeviceMemoryUploader::create_staging_buffer_description(const RHIStagingBufferContext& context) {
    RHIBufferDescription staging_buffer_description = {};
    staging_buffer_description.sharing_mode = RHISharingMode::Shared;
    staging_buffer_description.usage = RHIBufferUsageFlags::TransferSrc;
    staging_buffer_description.memory_usage = RHIMemoryUsage::Host;
    staging_buffer_description.size = context.size;
    return staging_buffer_description;
}

RHIBufferDescription RHIDeviceMemoryUploader::create_buffer_description(const RHIStagingBufferContext& context) {
    RHIBufferDescription buffer_description = {};
    buffer_description.sharing_mode = RHISharingMode::Shared;
    buffer_description.usage = context.usage | RHIBufferUsageFlags::TransferDst;
    buffer_description.memory_usage = RHIMemoryUsage::Device;
    buffer_description.size = context.size;
    return buffer_description;
}

RHITexture* RHIDeviceMemoryUploader::send_texture(const RHIStagingBufferContext& context, RHITextureDescription& description) {
    RHIBufferDescription staging_buffer_description = create_staging_buffer_description(context);
    RHIBuffer* staging_buffer = staging_buffer_pool_->create_buffer(staging_buffer_description);

    staging_buffer->update(context.data, context.size);

    description.usage = description.usage | RHITextureUsageFlags::TransferDst;
    description.memory_usage = RHIMemoryUsage::Device;
    RHITexture* texture = device_->create_texture(description);

    texture_entries_.append(TextureEntry(staging_buffer, texture, context.size));

    return texture;
}

RHIBuffer* RHIDeviceMemoryUploader::send_buffer(const RHIStagingBufferContext& context) {
    RHIBufferDescription staging_buffer_description = create_staging_buffer_description(context);
    RHIBuffer* staging_buffer = staging_buffer_pool_->create_buffer(staging_buffer_description);

    staging_buffer->update(context.data, context.size);

    RHIBufferDescription buffer_description = create_buffer_description(context);
    RHIBuffer* buffer = buffer_pool_->create_buffer(buffer_description);

    buffer_entries_.append(BufferEntry(staging_buffer, buffer, context.size));

    return buffer;
}

// Upload Data from Standing Buffers to Device Buffers
void RHIDeviceMemoryUploader::upload() {
    // Fetch transfer queue.
    const Array<RHICommandQueueRef>& command_queues = device_->get_command_queues();
    RHICommandQueueRef* transfer_queue_ptr = command_queues.get_if([](RHICommandQueueRef queue) {
        return queue->is_transfer_type();
    });

    LCHECK_MSG(transfer_queue_ptr, "Found no transfer command queue.");
    RHICommandQueueRef transfer_queue = *transfer_queue_ptr;

    RHICommandAllocatorDescription transfer_command_allocator_desc = {};
    transfer_command_allocator_desc.count = 1;  // One command buffer allocated.
    transfer_command_allocator_desc.command_queue = transfer_queue;

    RHICommandAllocator* transfer_command_allocator = device_->create_command_allocator(transfer_command_allocator_desc);

    // Open a transfer command buffer
    RHICommandBuffer* transfer_cmd = transfer_command_allocator->open();
    transfer_command_allocator->reset_command_buffer(transfer_cmd);

    transfer_cmd->begin();
    {
        for (auto& [staging_buffer, buffer, size] : buffer_entries_) {
            RHICopyBufferCommand buffer_copy_command = {};
            buffer_copy_command.source = staging_buffer;
            buffer_copy_command.destination = buffer;
            buffer_copy_command.size = size;
            transfer_cmd->copy_buffer(buffer_copy_command);
        }

        for (auto& entry : texture_entries_) {
            RHITexture* texture = entry.texture;

            RHITextureBarrier barrier_to_copy = {};
            barrier_to_copy.texture = texture;
            barrier_to_copy.old_layout = RHITextureLayout::Undefined;
            barrier_to_copy.new_layout = RHITextureLayout::TransferDst;
            transfer_cmd->transition_texture(barrier_to_copy);

            RHICopyBufferToTextureCommand copy_cmd = {};
            copy_cmd.source = entry.staging;
            copy_cmd.destination = texture;
            transfer_cmd->copy_buffer_to_texture(copy_cmd);

            RHITextureBarrier barrier_to_final = {};
            barrier_to_final.texture = texture;
            barrier_to_final.old_layout = RHITextureLayout::TransferDst;
            barrier_to_final.new_layout = RHITextureLayout::ShaderReadOnly;

            transfer_cmd->transition_texture(barrier_to_final);
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
    staging_buffer_pool_->dispose();

    device_->destroy_command_allocator(transfer_command_allocator);
}

}  //namespace licht