#pragma once

#include "licht/core/math/vector3.hpp"
#include "licht/core/memory/linear_allocator.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/device.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/pipeline/pipeline.hpp"
#include "licht/rhi/swapchain.hpp"

namespace licht {

class RenderFrameScript {
public:
    void on_startup();

    void on_shutdown();

    void on_tick(float32 delta_time);

    void update_uniform();

    void update_resized(const uint32 width, const uint32 height);

    void pause();

    void unpause();

private:
    void reset();

public:
    RenderFrameScript();
    ~RenderFrameScript() = default;

private:
    using RHIFramebufferAllocator = TypedLinearAllocator<RHIFramebufferHandle, MemoryOwnership::NonOwner>;
    using RHIFramebufferRegistry = Array<RHIFramebufferHandle, RHIFramebufferAllocator>;

    RHIBufferHandle position_buffer_;
    Array<Vector3f> positions_;

    RHIBufferHandle color_buffer_;
    Array<Vector3f> colors_; 
        
    RHIBufferHandle index_buffer_;
    Array<uint32> indices_;

    Array<RHIBufferHandle> uniform_buffers_;

    WindowHandle window_handle_;
    RHIDeviceHandle device_;
    
    RHISwapchainHandle swapchain_;

    RHICommandQueueRef graphics_command_queue_;
    RHICommandQueueRef graphics_present_command_queue_;

    RHIRenderPassHandle render_pass_;
    RHIDescriptorPoolRef descriptor_pool_;
    RHIPipelineHandle graphics_pipeline_;

    LinearAllocator framebuffer_memory_allocator_;
    RHIFramebufferRegistry framebuffers_;

    RHICommandAllocatorRef graphics_command_allocator_;
    RHIFrameContext frame_context_;

    bool pause_ = false;
    bool window_resized_ = false;
};

}  //namespace licht