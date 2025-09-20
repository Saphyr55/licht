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
#include "scritable_tick.hpp"

namespace licht {

class RenderFrameScript : public ScriptableTick {
public:
    virtual void on_startup() override;

    virtual void on_shutdown() override;

    virtual void on_tick(float32 delta_time) override;

    void update_resized(const uint32 width, const uint32 height);

    void pause();

    void unpause();

private:
    void reset();

public:
    RenderFrameScript();
    virtual ~RenderFrameScript() override;

private:
    using RHIFramebufferAllocator = TypedLinearAllocator<RHIFramebufferHandle, MemoryOwnership::NonOwner>;
    using RHIFramebufferRegistry = Array<RHIFramebufferHandle, RHIFramebufferAllocator>;

    RHIBufferHandle position_buffer_;
    Array<Vector3f> positions_;

    RHIBufferHandle color_buffer_;
    Array<Vector3f> colors_; 

    WindowHandle window_handle_;
    RHIDeviceHandle device_;
    
    RHISwapchainHandle swapchain_;
    RHIRenderPassHandle render_pass_;
    RHIPipelineHandle pipeline_;

    LinearAllocator framebuffer_memory_allocator_;
    RHIFramebufferRegistry framebuffers_;

    RHICommandAllocatorRef command_allocator_;
    RHIFrameContext frame_context_;

    bool pause_ = false;
    bool window_resized_ = false;
};

}  //namespace licht