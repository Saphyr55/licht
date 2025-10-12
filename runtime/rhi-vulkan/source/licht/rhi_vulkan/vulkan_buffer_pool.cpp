#include "licht/rhi_vulkan/vulkan_buffer_pool.hpp"
#include "licht/core/defines.hpp"
#include "vulkan_buffer_pool.hpp"

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
    destroy_vulkan_buffer(static_cast<VulkanBuffer*>(buffer));
}

void VulkanBufferPool::dispose() {
    pool_.for_each([this](VulkanBuffer* buffer) -> void {
        destroy_vulkan_buffer(buffer);
    });
    pool_.dispose();
}

void VulkanBufferPool::destroy_vulkan_buffer(VulkanBuffer* vulkan_buffer) {
    vulkan_buffer->destroy();
    pool_.destroy_resource(vulkan_buffer);
}

}  //namespace licht