#include "licht/rhi_vulkan/vulkan_buffer_pool.hpp"

namespace licht {

void VulkanBufferPool::initialize_pool(Allocator* allocator, size_t block_count) {
    pool_.initialize_pool(allocator, block_count);
}

RHIBuffer* VulkanBufferPool::create_buffer(const RHIBufferDescription& description) {
    VulkanBuffer* vulkan_buffer = pool_.new_resource();
    vulkan_buffer->initialize(description);
    return vulkan_buffer;
}

void VulkanBufferPool::destroy_buffer(RHIBuffer* buffer) {
    VulkanBuffer* vulkan_buffer = static_cast<VulkanBuffer*>(buffer);
    vulkan_buffer->destroy();
    pool_.destroy_resource(vulkan_buffer);
}

}