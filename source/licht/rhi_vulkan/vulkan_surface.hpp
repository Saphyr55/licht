#pragma once

#include "licht/rhi_vulkan/vulkan_instance.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

struct VulkanContext;

class VulkanSurfaceLifetime {
public:
    virtual VkSurfaceKHR create(void* p_native_window_handle) = 0;

    virtual void destroy(VkSurfaceKHR p_surface) = 0;
};

class VulkanPlatformSurface {
private:
    VkSurfaceKHR handle_;
};

void vulkan_surface_init(VulkanContext* p_context, void* p_window_handle);

void vulkan_surface_destroy(VulkanContext* p_context);

}