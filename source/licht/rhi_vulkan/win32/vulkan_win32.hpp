#pragma once

#include "licht/rhi_vulkan/vulkan_surface.hpp"

namespace licht {

class VulkanWindowsSurface : public VulkanPlatformSurface {
public:
    virtual void initialize() override;

    virtual void destroy() override;

public:
    VulkanWindowsSurface(VulkanInstance& p_instance, VkAllocationCallbacks* allocator, void* p_window_handle);
};
    
}

