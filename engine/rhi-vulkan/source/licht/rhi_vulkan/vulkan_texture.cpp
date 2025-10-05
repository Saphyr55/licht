#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_texture.hpp"


#include <vulkan/vulkan_core.h>

namespace licht {

VkMemoryRequirements VulkanTexture::get_memory_requirements() {
    VkMemoryRequirements memory_requirements;
    VulkanAPI::lvkGetImageMemoryRequirements(context_.device, handle_, &memory_requirements);
    return memory_requirements;
}

void VulkanTexture::bind() {
    VulkanAPI::lvkBindImageMemory(context_.device, handle_, memory_, 0);
}

void VulkanTexture::initialize() {

    VkImageCreateInfo image_create_info = {};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.pNext = VK_NULL_HANDLE;
    image_create_info.imageType = type_;
    image_create_info.format = vulkan_format_get(description_.format);
    image_create_info.arrayLayers = description_.layers_count;
    image_create_info.extent.width = description_.width;
    image_create_info.extent.height = description_.height;
    image_create_info.extent.depth = description_.depth;
    image_create_info.mipLevels = description_.mipmap_levels;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = vulkan_texture_usage_get(description_.usage);
    image_create_info.sharingMode = vulkan_sharing_mode_get(description_.sharing_mode);
    image_create_info.pQueueFamilyIndices = queue_family_indices_.data();
    image_create_info.queueFamilyIndexCount = queue_family_indices_.size();

    VulkanAPI::lvkCreateImage(context_.device, &image_create_info, context_.allocator, &handle_);

    VkDeviceMemory device_memory;
    VkMemoryRequirements memory_requirements = get_memory_requirements();
    VkMemoryPropertyFlags memory_properties = vulkan_memory_usage_get(description_.memory_usage);

    VkMemoryAllocateInfo memory_allocation_info = {};
    memory_allocation_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocation_info.allocationSize = memory_requirements.size;
    memory_allocation_info.memoryTypeIndex = vulkan_find_memory_type(context_, memory_requirements.memoryTypeBits, memory_properties);

    VulkanAPI::lvkAllocateMemory(context_.device, &memory_allocation_info, context_.allocator, &device_memory);

    bind();
}

void VulkanTexture::destroy() {
    VulkanAPI::lvkDestroyImage(context_.device, handle_, context_.allocator);
}

}  //namespace licht