#pragma once

#include "licht/core/math/vector3.hpp"
#include "licht/core/memory/linear_allocator.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/rhi/buffer_pool.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/texture.hpp"
#include "ludo_types.hpp"

namespace licht {

class Camera;

class RenderFrameScript {
public:
    void on_startup();

    void on_shutdown();

    void on_tick(float64 delta_time);

    void update_uniform(float64 delta_time);

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

    RHISampler* orange_texture_sampler_;
    RHITexture* orange_texture_;
    RHITextureView* orange_texture_view_;

    RHIBuffer* position_buffer_;
    RHIBuffer* uv_buffer_;
    RHIBuffer* index_buffer_;
    CubeMesh cube_mesh_{};

    Array<RHIBuffer*> uniform_buffers_;

    WindowHandle window_handle_;
    RHIDeviceRef device_;
    RHIBufferPoolRef buffer_pool_;
    RHITexturePoolRef texture_pool_;

    RHISwapchain* swapchain_;

    RHICommandQueueRef graphics_command_queue_;
    RHICommandQueueRef graphics_present_command_queue_;

    RHIRenderPass* render_pass_;
    RHIShaderResourceGroupLayout* shader_resource_group_layout_;
    RHIShaderResourceGroupPool* shader_resource_pool_;
    RHIGraphicsPipeline* graphics_pipeline_;

    LinearAllocator framebuffer_memory_allocator_;
    Array<RHIFramebuffer*> framebuffers_;
    RHITextureView* depth_texture_view_ ;
    RHITexture* depth_texture_;

    RHICommandAllocator* graphics_command_allocator_;
    RHIFrameContext frame_context_;

    bool pause_ = false;
    bool window_resized_ = false;
};

}  //namespace licht