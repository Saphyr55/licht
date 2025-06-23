#include "vulkan_swapchain.hpp"
#include "licht/core/collection/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_context.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_framebuffer.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

struct VulkanSwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    Array<VkSurfaceFormatKHR> surface_formats;
    Array<VkPresentModeKHR> present_modes;
};

uint32 vulkan_swapchain_count_image(const VkSurfaceCapabilitiesKHR& p_capabilities) {
    uint32 image_count = p_capabilities.minImageCount + 1;

    if (p_capabilities.maxImageCount > 0 && image_count > p_capabilities.maxImageCount) {
        image_count = p_capabilities.maxImageCount;
    }

    return image_count;
}

VkExtent2D vulkan_choose_swap_extent(const VkSurfaceCapabilitiesKHR& p_capabilities) {
    return p_capabilities.currentExtent;
}

VkPresentModeKHR vulkan_choose_swap_present_mode(const Array<VkPresentModeKHR>& p_available_present_modes) {
    if (p_available_present_modes.contains(VK_PRESENT_MODE_MAILBOX_KHR)) {
        return VK_PRESENT_MODE_MAILBOX_KHR;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR vulkan_choose_swap_surface_format(const Array<VkSurfaceFormatKHR>& p_available_surface_formats) {
    auto is_priority_format = [](const VkSurfaceFormatKHR& p_available_format) -> bool {
        return p_available_format.format == VK_FORMAT_B8G8R8A8_SRGB && p_available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    };

    if (const VkSurfaceFormatKHR* surface_format = p_available_surface_formats.get_if(is_priority_format)) {
        return *surface_format;
    }

    return p_available_surface_formats[0];
}

VulkanSwapchainSupportDetails vulkan_query_swapchain_support_details(VulkanContext* p_context) {
    LCHECK(p_context)

    VulkanSwapchainSupportDetails swapchain_support_details = {};

    VkPhysicalDevice physical_device = p_context->physical_device.handle;

    LICHT_VULKAN_CHECK(p_context->api.licht_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, p_context->surface, &swapchain_support_details.capabilities));

    uint32 format_count = 0;
    LICHT_VULKAN_CHECK(p_context->api.licht_vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, p_context->surface, &format_count, nullptr));

    if (format_count != 0) {
        swapchain_support_details.surface_formats.resize(format_count);
        LICHT_VULKAN_CHECK(p_context->api.licht_vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, p_context->surface, &format_count, swapchain_support_details.surface_formats.data()));
    }

    uint32 present_mode_count = 0;
    LICHT_VULKAN_CHECK(p_context->api.licht_vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, p_context->surface, &present_mode_count, nullptr));

    if (present_mode_count != 0) {
        swapchain_support_details.present_modes.resize(present_mode_count);
        LICHT_VULKAN_CHECK(p_context->api.licht_vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, p_context->surface, &present_mode_count, swapchain_support_details.present_modes.data()));
    }

    return swapchain_support_details;
}

void vulkan_context_swapchain_image_views_init(VulkanContext* p_context) {
    p_context->swapchain_image_views.resize(p_context->swapchain_images.size());
    for (usize i = 0; i < p_context->swapchain_images.size(); i++) {
        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = p_context->swapchain_images[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = p_context->swapchain_format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;
        LICHT_VULKAN_CHECK(p_context->api.licht_vkCreateImageView(p_context->device, &image_view_create_info, p_context->allocator, &p_context->swapchain_image_views[i]))
    }
}

void vulkan_swapchain_init(VulkanContext* p_context) {
    LCHECK(p_context)

    VulkanSwapchainSupportDetails swapchain_support_details = vulkan_query_swapchain_support_details(p_context);

    bool swapchain_adequate = !swapchain_support_details.surface_formats.empty() && !swapchain_support_details.present_modes.empty();

    VkSurfaceFormatKHR surface_format = vulkan_choose_swap_surface_format(swapchain_support_details.surface_formats);
    VkPresentModeKHR present_mode = vulkan_choose_swap_present_mode(swapchain_support_details.present_modes);
    VkExtent2D extent = vulkan_choose_swap_extent(swapchain_support_details.capabilities);
    uint32 image_count = vulkan_swapchain_count_image(swapchain_support_details.capabilities);

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = p_context->surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = surface_format.colorSpace;
    swapchain_create_info.imageExtent = extent;
    swapchain_create_info.imageArrayLayers = 1;
    // TODO: use VK_IMAGE_USAGE_TRANSFER_DST_BIT mask for post-processing thing in the future
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32 queue_famillies[] = { p_context->physical_device.graphics_queue_index, p_context->physical_device.present_queue_index }; 

    if (queue_famillies[0] != queue_famillies[1]) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_famillies; 
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0; // Optional
        swapchain_create_info.pQueueFamilyIndices = nullptr; // Optional
    }
    
    swapchain_create_info.preTransform = swapchain_support_details.capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    LICHT_VULKAN_CHECK(p_context->api.licht_vkCreateSwapchainKHR(p_context->device, &swapchain_create_info, p_context->allocator, &p_context->swapchain));

    image_count = 0;
    p_context->api.licht_vkGetSwapchainImagesKHR(p_context->device, p_context->swapchain, &image_count, nullptr);
    p_context->swapchain_images.resize(image_count);
    p_context->api.licht_vkGetSwapchainImagesKHR(p_context->device, p_context->swapchain, &image_count, p_context->swapchain_images.data());

    p_context->swapchain_extent = extent;
    p_context->swapchain_format = surface_format.format;

    vulkan_context_swapchain_image_views_init(p_context);
}

void vulkan_swapchain_recreate(VulkanContext* p_context) {
    LCHECK(p_context);
    
    LICHT_VULKAN_CHECK(p_context->api.licht_vkDeviceWaitIdle(p_context->device));

    vulkan_framebuffers_destroy(p_context);

    vulkan_swapchain_destroy(p_context);
    vulkan_swapchain_init(p_context);

    vulkan_framebuffers_init(p_context);
}

void vulkan_swapchain_destroy(VulkanContext* p_context) {
    LCHECK(p_context);

    for (const VkImageView& image_view : p_context->swapchain_image_views) {
        p_context->api.licht_vkDestroyImageView(p_context->device, image_view, p_context->allocator);
    }

    p_context->swapchain_image_views.clear();
    p_context->api.licht_vkDestroySwapchainKHR(p_context->device, p_context->swapchain, p_context->allocator);
}

}  //namespace licht