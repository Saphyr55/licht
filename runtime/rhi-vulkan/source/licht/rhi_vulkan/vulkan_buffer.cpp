
#include "licht/rhi_vulkan/vulkan_buffer.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

VkMemoryRequirements VulkanBuffer::get_memory_requirements() {
    VulkanContext& context_ = vulkan_context_get();

    VkMemoryRequirements memory_requirements;
    VulkanAPI::lvkGetBufferMemoryRequirements(context_.device, buffer_, &memory_requirements);
    return memory_requirements;
}

Array<uint32> VulkanBuffer::get_queue_family_indices() {
    VulkanContext& context_ = vulkan_context_get();

    Array<uint32> indices;
    if (description_.sharing_mode == RHISharingMode::Shared) {
        if ((description_.usage & RHIBufferUsageFlags::TransferDst) == RHIBufferUsageFlags::TransferDst ||
            (description_.usage & RHIBufferUsageFlags::TransferSrc) == RHIBufferUsageFlags::TransferSrc) {
            indices = vulkan_query_queue_family_indices(context_);
        }
    }
    return indices;
}

void VulkanBuffer::initialize(const RHIBufferDescription& description, VkBuffer buffer) {
    buffer_ = buffer;
    initialize(description);
}

void VulkanBuffer::initialize(const RHIBufferDescription& description) {
    description_ = description;

    VulkanContext& context_ = vulkan_context_get();

    Array<uint32> indices = get_queue_family_indices();

    VkBufferCreateInfo buffer_create_info = {};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.pNext = VK_NULL_HANDLE;
    buffer_create_info.queueFamilyIndexCount = indices.size();
    buffer_create_info.pQueueFamilyIndices = indices.data();
    buffer_create_info.size = description_.size;
    buffer_create_info.usage = vulkan_buffer_usage_get(description_.usage);
    buffer_create_info.sharingMode = vulkan_sharing_mode_get(description_.sharing_mode);
    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateBuffer(context_.device, &buffer_create_info, context_.allocator, &buffer_));

    VkMemoryRequirements memory_requirements = get_memory_requirements();
    VkMemoryPropertyFlags properties = vulkan_memory_usage_get(description_.memory_usage);

    VkMemoryAllocateInfo memory_allocate_info = {};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.pNext = VK_NULL_HANDLE;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = vulkan_find_memory_type(context_, memory_requirements.memoryTypeBits, properties);

    LICHT_VULKAN_CHECK(VulkanAPI::lvkAllocateMemory(context_.device, &memory_allocate_info, context_.allocator, &memory_));

    bind();
}

void VulkanBuffer::destroy() {
    VulkanContext& context_ = vulkan_context_get();

    VulkanAPI::lvkDestroyBuffer(context_.device, buffer_, context_.allocator);
    VulkanAPI::lvkFreeMemory(context_.device, memory_, context_.allocator);
}

void VulkanBuffer::bind() {
    VulkanContext& context_ = vulkan_context_get();

    LICHT_VULKAN_CHECK(VulkanAPI::lvkBindBufferMemory(context_.device, buffer_, memory_, 0));
}

void* VulkanBuffer::map(size_t offset, size_t size) {
    VulkanContext& context_ = vulkan_context_get();

    void* data = nullptr;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkMapMemory(context_.device, memory_, offset, size, 0, &data));
    return data;
}

void VulkanBuffer::unmap() {
    VulkanContext& context_ = vulkan_context_get();
    VulkanAPI::lvkUnmapMemory(context_.device, memory_);
}

void VulkanBuffer::update(const void* source, size_t size, size_t offset) {
    VulkanContext& context_ = vulkan_context_get();
    void* destination = map(offset, size);
    if (destination) {
        Memory::copy(destination, source, size);
        unmap();
    }
}

RHISharingMode VulkanBuffer::get_sharing_mode() {
    return description_.sharing_mode;
}

size_t VulkanBuffer::get_size() {
    return description_.size;
}

RHIBufferUsageFlags VulkanBuffer::get_usage() {
    return description_.usage;
}

VkBuffer VulkanBuffer::get_handle() {
    return buffer_;
}

}  //namespace licht