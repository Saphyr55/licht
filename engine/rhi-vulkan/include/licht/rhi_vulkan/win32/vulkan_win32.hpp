#pragma once

#include "licht/rhi_vulkan/vulkan_render_surface.hpp"

namespace licht {

class RHIVulkanWindowsSurface : public RHIVulkanRenderSurface {
public:
    virtual void initialize() override;

    virtual void destroy() override;

public:
    RHIVulkanWindowsSurface(VulkanContext& context, void* p_window_handle);
};
    
}

