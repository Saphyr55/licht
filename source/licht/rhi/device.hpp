#pragma once

#include "licht/core/collection/array.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/pipeline/graphics_pipeline.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/render_surface.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/texture.hpp"

namespace licht {

class RHIDevice {
public:
    virtual RHIGraphicsPipelineHandle create_graphics_pipeline(RHIPipelineDescription description) = 0;

    virtual RHIBufferHandle create_buffer() = 0;

    virtual RHICommandBufferHandle create_command_buffer() = 0;

    virtual RHISwapchainHandle create_swapchain(RHIRenderSurfaceHandle render_surface) = 0;

    virtual void recreate_swapchain(RHISwapchainHandle swapchain) = 0;

    virtual RHIFramebufferHandle create_framebuffer(RHIRenderPassHandle render_pass,
                                                    const Array<RHITextureViewHandle>& texture_views,
                                                    const RHIFramebufferDescription& description) = 0;

    virtual void destroy_framebuffer(RHIFramebufferHandle framebuffer) = 0;
};

using RHIDeviceHandle = SharedRef<RHIDevice>;

}  //namespace licht