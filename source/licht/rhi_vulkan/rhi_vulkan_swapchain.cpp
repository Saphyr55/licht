#include "rhi_vulkan_swapchain.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_device.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

namespace licht {

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

RHIVulkanSwapchain::RHIVulkanSwapchain(VulkanContext& context, SharedRef<RHIVulkanRenderSurface> surface)
    : context_(context), surface_(surface) {
}

VulkanSwapchainSupportDetails RHIVulkanSwapchain::query_support_details() {
    VulkanSwapchainSupportDetails swapchain_support_details = {};

    VkPhysicalDevice physical_device = context_.physical_device;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface_->get_handle(), &swapchain_support_details.capabilities));

    uint32 format_count = 0;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface_->get_handle(), &format_count, nullptr));

    if (format_count != 0) {
        swapchain_support_details.surface_formats.resize(format_count);
        LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface_->get_handle(), &format_count, swapchain_support_details.surface_formats.data()));
    }

    uint32 present_mode_count = 0;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface_->get_handle(), &present_mode_count, nullptr));

    if (present_mode_count != 0) {
        swapchain_support_details.present_modes.resize(present_mode_count);
        LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface_->get_handle(), &present_mode_count, swapchain_support_details.present_modes.data()));
    }

    return swapchain_support_details;
}

void RHIVulkanSwapchain::image_views_init() {
    image_views_.resize(images_.size());
    for (usize i = 0; i < images_.size(); i++) {
        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = images_[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = format_;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;
        LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateImageView(context_.device, &image_view_create_info, context_.allocator, &image_views_[i]))
    }
}

void RHIVulkanSwapchain::initialize() {
    VulkanSwapchainSupportDetails swapchain_support_details = query_support_details();

    bool swapchain_adequate = !swapchain_support_details.surface_formats.empty() && !swapchain_support_details.present_modes.empty();

    VkSurfaceFormatKHR surface_format = vulkan_choose_swap_surface_format(swapchain_support_details.surface_formats);
    VkPresentModeKHR present_mode = vulkan_choose_swap_present_mode(swapchain_support_details.present_modes);
    VkExtent2D extent = vulkan_choose_swap_extent(swapchain_support_details.capabilities);
    uint32 image_count = vulkan_swapchain_count_image(swapchain_support_details.capabilities);

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = surface_->get_handle();
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = surface_format.colorSpace;
    swapchain_create_info.imageExtent = extent;
    swapchain_create_info.imageArrayLayers = 1;
    // TODO: use VK_IMAGE_USAGE_TRANSFER_DST_BIT mask for post-processing for the future.
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32 graphics_queue_index = context_.physical_device_info.graphics_queue_index;
    uint32 present_queue_index = context_.physical_device_info.present_queue_index;
    uint32 queue_famillies[] = {graphics_queue_index, present_queue_index};

    if (queue_famillies[0] != queue_famillies[1]) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_famillies;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;      // Optional
        swapchain_create_info.pQueueFamilyIndices = nullptr;  // Optional
    }

    swapchain_create_info.preTransform = swapchain_support_details.capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateSwapchainKHR(context_.device, &swapchain_create_info, context_.allocator, &handle_));

    image_count = 0;
    VulkanAPI::lvkGetSwapchainImagesKHR(context_.device, handle_, &image_count, nullptr);

    images_.resize(image_count);
    VulkanAPI::lvkGetSwapchainImagesKHR(context_.device, handle_, &image_count, images_.data());

    extent_ = extent;
    format_ = surface_format.format;

    image_views_init();
}

void RHIVulkanSwapchain::destroy() {
    for (const VkImageView& image_view : image_views_) {
        VulkanAPI::lvkDestroyImageView(context_.device, image_view, context_.allocator);
    }

    image_views_.clear();
    VulkanAPI::lvkDestroySwapchainKHR(context_.device, handle_, context_.allocator);
}

void RHIVulkanSwapchain::present() {
}

uint32 RHIVulkanSwapchain::get_width() {
    return extent_.width;
}

uint32 RHIVulkanSwapchain::get_height() {
    return extent_.height;
}

}  //namespace licht