#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi_vulkan/vulkan_render_surface.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

struct VulkanSwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    Array<VkSurfaceFormatKHR> surface_formats;
    Array<VkPresentModeKHR> present_modes;
};

class VulkanSwapchain : public RHISwapchain {
public:
    virtual void acquire_next_frame(RHIFrameContext& context) override;

    virtual uint32 get_width() override;

    virtual uint32 get_height() override;

    virtual RHIFormat get_format() override;

    virtual const Array<RHITextureView*>& get_texture_views() const override {
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

    void set_extent(VkExtent2D extent) {
        extent_ = extent;        
    }

public:
    VulkanSwapchain(VulkanContext& context, uint32 width, uint32 height, uint32 image_count);

private:
    VulkanSwapchainSupportDetails query_support_details();

private:
    VulkanContext& context_;
    VkFormat format_;
    VkExtent2D extent_;
    Array<VkImage> images_;
    Array<RHITextureView*> texture_views_;
    VkSwapchainKHR handle_ = VK_NULL_HANDLE;
    uint32 image_count_ = 3;
};

using RHIVulkanSwapchainRef = SharedRef<VulkanSwapchain>;

}  //namespace licht