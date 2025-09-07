#pragma once

#include "licht/core/collection/array.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/rhi.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/texture.hpp"
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
    virtual void acquire_next_frame(RHIFrameContext& context) override;

    virtual uint32 get_width() override;

    virtual uint32 get_height() override;

    virtual RHIFormat get_format() override;

    virtual const Array<RHITextureViewHandle>& get_texture_views() override {
        return texture_views_;
    }

    void initialize();

    void destroy();

public:
    inline VkFormat get_vkformat() {
        return format_;
    }

    inline VkSwapchainKHR& get_handle() {
        return handle_;
    }

    inline VkExtent2D get_extent() {
        return extent_;
    }

public:
    RHIVulkanSwapchain(VulkanContext& context);

private:
    VulkanSwapchainSupportDetails query_support_details();

private:
    VulkanContext& context_;
    VkFormat format_;
    VkExtent2D extent_;
    Array<VkImage> images_;
    Array<RHITextureViewHandle> texture_views_;
    VkSwapchainKHR handle_ = VK_NULL_HANDLE;
};

using RHIVulkanSwapchainRef = SharedRef<RHIVulkanSwapchain>;

}  //namespace licht