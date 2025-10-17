#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/renderer/render_item.hpp"
#include "licht/renderer/render_context.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/texture.hpp"
#include "material_graphics_pipeline.hpp"

namespace licht {

class Camera;

class RenderFrameScript {
public:
    void on_startup();

    void on_shutdown();

    void on_tick(float64 delta_time);

    void update_resized(const uint32 width, const uint32 height);

    void pause();

    void unpause();

private:
    void update_uniform(float64 delta_time);

    void reset();

public:
    RenderFrameScript(Camera* camera);
    ~RenderFrameScript() = default;

private:
    Camera* camera_;

    RenderPacket packet_;

    WindowHandle window_handle_;
    RHIDeviceRef device_;
    RHIBufferPoolRef buffer_pool_;
    RHITexturePoolRef texture_pool_;

    RHICommandQueueRef graphics_queue_;
    RHICommandQueueRef present_queue_;
    
    Array<RHIFramebuffer*> framebuffers_;
    RHITextureView* depth_texture_view_ ;
    RHITexture* depth_texture_;

    RHICommandAllocator* cmd_allocator_;

    SharedRef<RenderContext> renderer_;
    SharedRef<MaterialGraphicsPipeline> material_graphics_pipeline_;

    bool pause_ = false;
};

}  //namespace licht