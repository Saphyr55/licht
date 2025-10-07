#include "licht/rhi_vulkan/vulkan_texture_view.hpp"
#include "licht/rhi_vulkan/vulkan_texture.hpp"

namespace licht {

void VulkanTextureView::initialize(const RHITextureViewDescription& description, VkImageView image_view) {
    handle_ = image_view;

    VulkanContext& context = vulkan_context_get();
    VulkanTexture* texture = static_cast<VulkanTexture*>(description_.texture);

    VkImageViewType image_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    switch (description_.dimension) {
        case RHITextureDimension::Dim3D:
            image_type = VK_IMAGE_VIEW_TYPE_3D;
            break;
        case RHITextureDimension::Dim2D:
            image_type = VK_IMAGE_VIEW_TYPE_2D;
            break;
        case RHITextureDimension::Dim1D:
            image_type = VK_IMAGE_VIEW_TYPE_1D;
            break;
        default:
            image_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
            break;
    }

    VkImageViewCreateInfo image_view_create_info{};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = texture->get_handle();
    image_view_create_info.viewType = image_type;
    image_view_create_info.format = vulkan_format_get(description_.format);
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = description_.mip_level;
    image_view_create_info.subresourceRange.levelCount = description_.mip_level_count;
    image_view_create_info.subresourceRange.baseArrayLayer = description_.array_layer;
    image_view_create_info.subresourceRange.layerCount = description_.array_layer_count;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateImageView(context.device, &image_view_create_info, context.allocator, &handle_));
}

void VulkanTextureView::initialize(const RHITextureViewDescription& description) {
    initialize(description, handle_);
}

void VulkanTextureView::destroy() {
    VulkanContext& context = vulkan_context_get();
    VulkanAPI::lvkDestroyImageView(context.device, handle_, context.allocator);
}

}