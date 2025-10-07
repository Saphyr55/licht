#include "licht/rhi_vulkan/vulkan_texture.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

VkMemoryRequirements VulkanTexture::get_memory_requirements() {
    VulkanContext& context = vulkan_context_get();
    VkMemoryRequirements memory_requirements;
    VulkanAPI::lvkGetImageMemoryRequirements(context.device, handle_, &memory_requirements);
    return memory_requirements;
}

void VulkanTexture::bind() {
    VulkanContext& context = vulkan_context_get();
    VulkanAPI::lvkBindImageMemory(context.device, handle_, memory_, 0);
}

void VulkanTexture::initialize(const RHITextureDescription& desc) {
    description_ = desc;
    VulkanContext& context = vulkan_context_get();
    VkImageType image_type = VK_IMAGE_TYPE_MAX_ENUM;
    switch (description_.dimension) {
        case RHITextureDimension::Dim3D:
            image_type = VK_IMAGE_TYPE_3D;
            break;
        case RHITextureDimension::Dim2D:
            image_type = VK_IMAGE_TYPE_2D;
            break;
        case RHITextureDimension::Dim1D:
            image_type = VK_IMAGE_TYPE_1D;
            break;
        default:
            image_type = VK_IMAGE_TYPE_MAX_ENUM;
            break;
    }

    Array<uint32> queue_famillies = vulkan_query_queue_family_indices(context);

    VkImageCreateInfo image_create_info = {};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.pNext = VK_NULL_HANDLE;
    image_create_info.imageType = image_type;
    image_create_info.format = vulkan_format_get(desc.format);
    image_create_info.arrayLayers = desc.array_layers;
    image_create_info.mipLevels = desc.mip_levels;
    image_create_info.extent.width = desc.width;
    image_create_info.extent.height = desc.height;
    image_create_info.extent.depth = desc.depth;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = vulkan_texture_usage_get(desc.usage);
    image_create_info.sharingMode = vulkan_sharing_mode_get(desc.sharing_mode);
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;

    if (RHISharingMode::Private == desc.sharing_mode) {
        image_create_info.pQueueFamilyIndices = nullptr;
        image_create_info.queueFamilyIndexCount = 0;
    } else if (RHISharingMode::Shared == desc.sharing_mode) {
        image_create_info.pQueueFamilyIndices = queue_famillies.data();
        image_create_info.queueFamilyIndexCount = static_cast<uint32>(queue_famillies.size());
    }

    VulkanAPI::lvkCreateImage(context.device, &image_create_info, context.allocator, &handle_);

    VkMemoryRequirements memory_requirements = get_memory_requirements();
    VkMemoryPropertyFlags memory_properties = vulkan_memory_usage_get(desc.memory_usage);

    VkMemoryAllocateInfo memory_allocation_info = {};
    memory_allocation_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocation_info.allocationSize = memory_requirements.size;
    memory_allocation_info.memoryTypeIndex = vulkan_find_memory_type(context, memory_requirements.memoryTypeBits, memory_properties);

    VulkanAPI::lvkAllocateMemory(context.device, &memory_allocation_info, context.allocator, &memory_);

    bind();
}

void VulkanTexture::destroy() {
    VulkanContext& context = vulkan_context_get();
    VulkanAPI::lvkDestroyImage(context.device, handle_, context.allocator);
    VulkanAPI::lvkFreeMemory(context.device, memory_, context.allocator);
}

}  //namespace licht