#include "licht/rhi_vulkan/vulkan_texture.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

VkMemoryRequirements RHIVulkanTexture::get_memory_requirements(VulkanContext& context, VkImage image) {
    VkMemoryRequirements memory_requirements;
    VulkanAPI::lvkGetImageMemoryRequirements(context.device, image, &memory_requirements);
    return memory_requirements;
}

SharedRef<RHIVulkanTexture> RHIVulkanTexture::create(VulkanContext& context,
                                                     const RHITextureDescription& description,
                                                     VkImageType type,
                                                     VkImageUsageFlags usage,
                                                     VkSharingMode sharing_mode,
                                                     const Array<uint32> queue_family_indices) {
    VkImage image;
    VkImageCreateInfo image_create_info = {};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.pNext = VK_NULL_HANDLE;
    image_create_info.imageType = type;
    image_create_info.format = vulkan_format_get(description.format);
    image_create_info.arrayLayers = description.layers_count;
    image_create_info.extent.width = description.width;
    image_create_info.extent.height = description.height;
    image_create_info.extent.depth = description.depth;
    image_create_info.mipLevels = description.mipmap_levels;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = usage;
    image_create_info.sharingMode = sharing_mode;
    image_create_info.pQueueFamilyIndices = queue_family_indices.data();
    image_create_info.queueFamilyIndexCount = queue_family_indices.size();

    VulkanAPI::lvkCreateImage(context.device, &image_create_info, context.allocator, &image);

    VkDeviceMemory device_memory;
    VkMemoryRequirements memory_requirements = get_memory_requirements(context, image);

    VkMemoryAllocateInfo memory_allocation_info = {};
    memory_allocation_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocation_info.allocationSize = memory_requirements.size;
    memory_allocation_info.memoryTypeIndex = vulkan_find_memory_type(context, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VulkanAPI::lvkAllocateMemory(context.device, &memory_allocation_info, context.allocator, &device_memory);

    return new_ref<RHIVulkanTexture>(context, image, device_memory);
}

void RHIVulkanTexture::bind() {
    VulkanAPI::lvkBindImageMemory(context_.device, handle_, memory_, 0);
}

void RHIVulkanTexture::destroy(VulkanContext& context, RHIVulkanTexture& texture) {
    VulkanAPI::lvkDestroyImage(context.device, texture.get_handle(), context.allocator);
}

}  //namespace licht