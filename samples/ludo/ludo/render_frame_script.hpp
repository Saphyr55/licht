#pragma once

#include "licht/core/math/vector3.hpp"
#include "licht/core/memory/linear_allocator.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/rhi/swapchain.hpp"

namespace licht {

class Camera;

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
    RenderFrameScript(Camera* camera);
    ~RenderFrameScript() = default;

private:
    Camera* camera_;

    RHIBufferRef position_buffer_;
    Array<Vector3f> positions_;

    RHIBufferRef color_buffer_;
    Array<Vector3f> colors_; 
        
    RHIBufferRef index_buffer_;
    Array<uint32> indices_;

    Array<RHIBufferRef> uniform_buffers_;

    WindowHandle window_handle_;
    RHIDeviceRef device_;
    
    RHISwapchainRef swapchain_;

    RHICommandQueueRef graphics_command_queue_;
    RHICommandQueueRef graphics_present_command_queue_;

    RHIRenderPassRef render_pass_;
    RHIDescriptorPoolRef descriptor_pool_;
    RHIPipelineRef graphics_pipeline_;

    LinearAllocator framebuffer_memory_allocator_;
    Array<RHIFramebufferRef> framebuffers_;

    RHICommandAllocatorRef graphics_command_allocator_;
    RHIFrameContext frame_context_;

    bool pause_ = false;
    bool window_resized_ = false;
};

}  //namespace licht