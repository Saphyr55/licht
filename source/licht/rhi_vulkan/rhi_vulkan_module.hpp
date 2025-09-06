#pragma once

#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/pipeline/pipeline.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/render_surface.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_exports.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

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
    
    RHIRenderSurfaceHandle surface_;
    RHIDeviceHandle device_;
    RHISwapchainHandle swapchain_;
    RHIRenderPassHandle render_pass_;
    RHIPipelineHandle pipeline_;
    Array<RHIFramebufferHandle> framebuffers_;
    RHICommandAllocatorRef command_allocator_;
    RHIFrameContext frame_context_;

    bool window_resized_ = false;
};

}