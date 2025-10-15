#pragma once

#include "licht/core/memory/linear_allocator.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/renderer/renderer.hpp"
#include "licht/renderer/mesh/attribute.hpp"
#include "licht/rhi/buffer_pool.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "licht/rhi/swapchain.hpp"
#include "licht/rhi/texture.hpp"
#include "ludo_types.hpp"

namespace licht {

class Camera;

struct RenderSubMesh {
    Array<RHIBuffer*> vertex_buffers;

    RHIBuffer* index_buffer;
    size_t index_count = 0;

    RHISampler* sampler;

    RHITexture* texture;
    RHITextureView* texture_view;

    Array<RHIShaderResourceGroup*> shader_groups;
};

struct RenderMesh {
    Array<RenderSubMesh> submeshes;
};

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

    Array<RenderMesh> render_model_;

    Array<RHIBuffer*> uniform_buffers_;

    WindowHandle window_handle_;
    RHIDeviceRef device_;
    RHIBufferPoolRef buffer_pool_;
    RHITexturePoolRef texture_pool_;

    RHICommandQueueRef graphics_queue_;
    RHICommandQueueRef present_queue_;
    
    RHIShaderResourceBinding sampler_binding_;

    RHIRenderPass* render_pass_;
    RHIShaderResourceGroupLayout* shader_resource_group_layout_;
    RHIShaderResourceGroupPool* shader_resource_pool_;
    RHIGraphicsPipeline* graphics_pipeline_;

    Array<RHIFramebuffer*> framebuffers_;
    RHITextureView* depth_texture_view_ ;
    RHITexture* depth_texture_;

    RHICommandAllocator* cmd_allocator_;

    SharedRef<Renderer> renderer_;

    bool pause_ = false;
};

}  //namespace licht