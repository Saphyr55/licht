#include "licht/rhi_vulkan/vulkan_swapchain.hpp"
#include "licht/core/math/math.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/vulkan_sync.hpp"
#include "licht/rhi_vulkan/vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_texture.hpp"
#include "licht/rhi_vulkan/vulkan_texture_view.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

uint32 vulkan_swapchain_count_image(const VkSurfaceCapabilitiesKHR& capabilities) {
    uint32 image_count = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }

    return image_count;
}

VkExtent2D vulkan_choose_swapchain_extent(VkExtent2D extent, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        extent = capabilities.currentExtent;
    }

    VkExtent2D min = capabilities.minImageExtent;
    VkExtent2D max = capabilities.maxImageExtent;

    extent.width = Math::clamp(extent.width, min.width, max.width);
    extent.height = Math::clamp(extent.height, min.height, max.height);

    return extent;
}

VkPresentModeKHR vulkan_choose_swapresent_mode(const Array<VkPresentModeKHR>& available_present_modes) {
    if (available_present_modes.contains(VK_PRESENT_MODE_MAILBOX_KHR)) {
        return VK_PRESENT_MODE_MAILBOX_KHR;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR vulkan_choose_swasurface_format(const Array<VkSurfaceFormatKHR>& available_surface_formats) {
    auto is_priority_format = [](const VkSurfaceFormatKHR& available_format) -> bool {
        return available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    };

    if (const VkSurfaceFormatKHR* surface_format = available_surface_formats.get_if(is_priority_format)) {
        return *surface_format;
    }

    return available_surface_formats[0];
}

VulkanSwapchain::VulkanSwapchain(VulkanContext& context, uint32 width, uint32 height, uint32 image_count)
    : context_(context)
    , extent_({width, height})
    , format_(VK_FORMAT_B8G8R8A8_SRGB)
    , image_count_(image_count) {
}

RHIFormat VulkanSwapchain::get_format() {
    return rhi_format_get(format_);
}

VulkanSwapchainSupportDetails VulkanSwapchain::query_support_details() {
    VulkanSwapchainSupportDetails swapchain_support_details = {};

    VkPhysicalDevice physical_device = context_.physical_device;
    VkSurfaceKHR surface = context_.surface->get_handle();

    LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &swapchain_support_details.capabilities));

    uint32 format_count = 0;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr));

    if (format_count != 0) {
        swapchain_support_details.surface_formats.resize(format_count);
        LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, swapchain_support_details.surface_formats.data()));
    }

    uint32 present_mode_count = 0;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr));

    if (present_mode_count != 0) {
        swapchain_support_details.present_modes.resize(present_mode_count);
        LICHT_VULKAN_CHECK(VulkanAPI::lvkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, swapchain_support_details.present_modes.data()));
    }

    return swapchain_support_details;
}

void VulkanSwapchain::initialize() {
    VulkanSwapchainSupportDetails swapchain_support_details = query_support_details();

    bool swapchain_adequate = !swapchain_support_details.surface_formats.empty() && !swapchain_support_details.present_modes.empty();

    VkSurfaceFormatKHR surface_format = vulkan_choose_swasurface_format(swapchain_support_details.surface_formats);
    VkPresentModeKHR present_mode = vulkan_choose_swapresent_mode(swapchain_support_details.present_modes);
    VkExtent2D extent = vulkan_choose_swapchain_extent(extent_, swapchain_support_details.capabilities);
    uint32 min_image_count = vulkan_swapchain_count_image(swapchain_support_details.capabilities);

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = context_.surface->get_handle();
    swapchain_create_info.minImageCount = min_image_count;
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

    uint32 image_count = 0;
    VulkanAPI::lvkGetSwapchainImagesKHR(context_.device, handle_, &image_count, nullptr);

    if (image_count_ >= image_count) {
        image_count_ = image_count;
    }

    images_.resize(image_count);
    texture_views_.reserve(image_count);

    VulkanAPI::lvkGetSwapchainImagesKHR(context_.device, handle_, &image_count, images_.data());

    extent_ = extent;
    format_ = surface_format.format;

    for (uint32 i = 0; i < image_count; i++) {
        VkImage image = images_[i];

        VulkanTextureView* texture_view = lnew(DefaultAllocator::get_instance(), VulkanTextureView());

        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = image;
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

        LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateImageView(context_.device, &image_view_create_info, context_.allocator, &texture_view->get_handle()))

        texture_views_.append(texture_view);
    }

    LCHECK(texture_views_.size() == images_.size());
}

void VulkanSwapchain::destroy() {
    for (RHITextureView* texture_view : texture_views_) {
        VulkanTextureView* vk_texture_view = static_cast<VulkanTextureView*>(texture_view);
        vk_texture_view->destroy();
    }

    texture_views_.clear();
    VulkanAPI::lvkDestroySwapchainKHR(context_.device, handle_, context_.allocator);
}

void VulkanSwapchain::acquire_next_frame(RHIFrameContext& context) {
    RHIVulkanSemaphore* frame_available_semaphore = static_cast<RHIVulkanSemaphore*>(context.current_frame_available_semaphore());

    VkResult acquire_next_image_result = VulkanAPI::lvkAcquireNextImageKHR(context_.device, handle_, UINT64_MAX, frame_available_semaphore->get_handle(), VK_NULL_HANDLE, &context.frame_index);
    switch (acquire_next_image_result) {
        case VK_SUCCESS: {
            context.success = true;
            return;
        }
        case VK_SUBOPTIMAL_KHR: {
            context.success = true;
            context.suboptimal = true;
            return;
        }
        case VK_ERROR_OUT_OF_DATE_KHR: {
            context.out_of_date = true;
            return;
        }
        default: {
            LICHT_VULKAN_CHECK(acquire_next_image_result);
            return;
        }
    }
}

uint32 VulkanSwapchain::get_width() {
    return extent_.width;
}

uint32 VulkanSwapchain::get_height() {
    return extent_.height;
}

}  //namespace licht