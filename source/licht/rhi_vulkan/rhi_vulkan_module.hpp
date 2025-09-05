#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/render_surface.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_device.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_exports.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"

namespace licht {

struct VulkanContext;

class LICHT_RHI_VULKAN_API RHIVulkanModule {
public:
    void initialize();

    void tick();

    void shutdown();

    inline void update_resized() {
        window_resized_ = true;
    }

private:
    VulkanContext context_;
    SharedRef<RHIVulkanRenderSurface> surface_;
    SharedRef<RHIVulkanDevice> device_;
    RHISwapchainHandle swapchain_;

    bool window_resized_ = false;
};

}