
#include "licht/rhi_vulkan/rhi_vulkan_buffer.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

static uint32 find_memory_type(VulkanContext& context, uint32 type_filter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    VulkanAPI::lvkGetPhysicalDeviceMemoryProperties(context.physical_device, &memory_properties);

    for (uint32 i = 0; i < memory_properties.memoryTypeCount; i++) {
        if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    LCRASH("Failed to find suitable Vulkan memory type for requested properties.")
    return 0;
}

RHIVulkanBuffer::RHIVulkanBuffer(VulkanContext& context, RHIBufferDescription description)
    : context_(context)
    , description_(description)
    , memory_(VK_NULL_HANDLE)
    , buffer_(VK_NULL_HANDLE) {
}

VkMemoryRequirements RHIVulkanBuffer::get_memory_requirements() {
    VkMemoryRequirements memory_requirements;
    VulkanAPI::lvkGetBufferMemoryRequirements(context_.device, buffer_, &memory_requirements);
    return memory_requirements;
}

VkMemoryPropertyFlags RHIVulkanBuffer::get_memory_property_flags() {
    VkMemoryPropertyFlags properties = 0;
    if (description_.memory_usage == RHIBufferMemoryUsage::Device) {
        properties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    } else if (description_.memory_usage == RHIBufferMemoryUsage::Host) {
        properties |= (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
    return properties;
}

Array<uint32> RHIVulkanBuffer::get_queue_family_indices() {
    Array<uint32> indices;
    if (description_.access_mode == RHIAccessMode::Shared) {
        if ((description_.usage & RHIBufferUsage::TransferDst) == RHIBufferUsage::TransferDst ||
            (description_.usage & RHIBufferUsage::TransferSrc) == RHIBufferUsage::TransferSrc) {
            indices = vulkan_query_queue_family_indices(context_);
        }
    }
    return indices;
}

void RHIVulkanBuffer::initialize() {

    Array<uint32> indices = get_queue_family_indices();

    VkBufferCreateInfo buffer_create_info = {};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.pNext = VK_NULL_HANDLE;
    buffer_create_info.queueFamilyIndexCount = indices.size();
    buffer_create_info.pQueueFamilyIndices = indices.data();
    buffer_create_info.size = description_.size;
    buffer_create_info.usage = vulkan_buffer_usage_get(description_.usage);
    buffer_create_info.sharingMode = vulkan_sharing_mode_get(description_.access_mode);
    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateBuffer(context_.device, &buffer_create_info, context_.allocator, &buffer_))

    VkMemoryRequirements memory_requirements = get_memory_requirements();
    VkMemoryPropertyFlags properties = get_memory_property_flags();

    VkMemoryAllocateInfo memory_allocate_info = {};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.pNext = VK_NULL_HANDLE;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = find_memory_type(context_, memory_requirements.memoryTypeBits, properties);

    LICHT_VULKAN_CHECK(VulkanAPI::lvkAllocateMemory(context_.device, &memory_allocate_info, context_.allocator, &memory_))

    bind();
}

void RHIVulkanBuffer::destroy() {
    VulkanAPI::lvkDestroyBuffer(context_.device, buffer_, context_.allocator);
    VulkanAPI::lvkFreeMemory(context_.device, memory_, context_.allocator);
}

void RHIVulkanBuffer::bind() {
    LICHT_VULKAN_CHECK(VulkanAPI::lvkBindBufferMemory(context_.device, buffer_, memory_, 0));
}

void* RHIVulkanBuffer::map(size_t offset, size_t size) {
    void* data = nullptr;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkMapMemory(context_.device, memory_, offset, size, 0, &data));
    return data;
}

void RHIVulkanBuffer::unmap() {
    VulkanAPI::lvkUnmapMemory(context_.device, memory_);
}

void RHIVulkanBuffer::update(const void* source, size_t size, size_t offset) {
    void* destination = map(offset, size);
    if (destination) {
        Memory::copy(destination, source, size);
        unmap();
    }
}

RHIAccessMode RHIVulkanBuffer::get_access_mode() {
    return description_.access_mode;
}

size_t RHIVulkanBuffer::get_size() {
    return description_.size;
}

RHIBufferUsage RHIVulkanBuffer::get_usage() {
    return description_.usage;
}

VkBuffer RHIVulkanBuffer::get_handle() {
    return buffer_;
}

}  //namespace licht