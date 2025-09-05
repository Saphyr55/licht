#pragma once

#include "licht/core/collection/array.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

struct VulkanSwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    Array<VkSurfaceFormatKHR> surface_formats;
    Array<VkPresentModeKHR> present_modes;
};

class RHIVulkanSwapchain : public RHISwapchain {
public:
    virtual void present() override;

    virtual uint32 get_width() override;

    virtual uint32 get_height() override;

    void initialize();

    void destroy();

public:
    inline VkSwapchainKHR get_handle() {
        return handle_;
    }

    inline VkFormat get_format() {
        return format_;
    }

    inline VkExtent2D get_extent() {
        return extent_;
    }

    inline Array<VkImageView>& get_image_views() {
        return image_views_;
    }

    inline const Array<VkImageView>& get_image_views() const {
        return image_views_;
    }

public:
    RHIVulkanSwapchain(VulkanContext& context, SharedRef<RHIVulkanRenderSurface> surface);

private:
    void image_views_init();

    VulkanSwapchainSupportDetails query_support_details();

private:
    VulkanContext& context_;
    SharedRef<RHIVulkanRenderSurface> surface_;
    VkSwapchainKHR handle_ = VK_NULL_HANDLE;
    VkFormat format_;
    VkExtent2D extent_;
    Array<VkImage> images_;
    Array<VkImageView> image_views_;
};

}  //namespace licht