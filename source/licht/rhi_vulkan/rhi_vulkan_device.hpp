#pragma once

#include "licht/core/collection/array.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

class RHIVulkanDevice : public RHIDevice {
public:
    virtual RHIGraphicsPipelineHandle create_graphics_pipeline(RHIPipelineDescription description) override;

    virtual RHIBufferHandle create_buffer() override;

    virtual RHICommandBufferHandle create_command_buffer() override;

    virtual RHISwapchainHandle create_swapchain(RHIRenderSurfaceHandle render_surface) override;

    virtual void recreate_swapchain(RHISwapchainHandle swapchain) override;

    virtual RHIFramebufferHandle create_framebuffer(RHIRenderPassHandle render_pass,
                                                    const Array<RHITextureViewHandle>& texture_views,
                                                    const RHIFramebufferDescription& description) override;

    virtual void destroy_framebuffer(RHIFramebufferHandle framebuffer) override;

public:
    RHIVulkanDevice(VulkanContext& context);

private:
    VulkanContext& context_; 
};

}  //namespace licht
