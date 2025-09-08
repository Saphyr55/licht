#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/linear_allocator.hpp"
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

    inline void update_resized(uint32 width, uint32 height) {
        window_resized_ = true;
        frame_context_.frame_width = width;
        frame_context_.frame_height = height;
    }

    inline void pause() {
        pause_ = true;
    }

    inline void unpause() {
        pause_ = false;
    }

public:
    RHIVulkanModule();

private:
    void reset();

private:
    using RHIFramebufferRegistryAllocator = LinearAllocator<RHIFramebufferHandle>;
    using RHIFramebufferRegistry = Array<RHIFramebufferHandle, RHIFramebufferRegistryAllocator>;
    
    VulkanContext context_;

    RHIRenderSurfaceHandle surface_;
    RHIDeviceHandle device_;
    RHISwapchainHandle swapchain_;
    RHIRenderPassHandle render_pass_;
    RHIPipelineHandle pipeline_;
    
    LinearMemoryPool framebuffer_memory_pool_;
    RHIFramebufferRegistry framebuffers_;
    
    RHICommandAllocatorRef command_allocator_;
    RHIFrameContext frame_context_;

    bool pause_ = false;
    bool window_resized_ = false;
};

}  //namespace licht