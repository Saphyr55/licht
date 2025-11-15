#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/renderer/draw_item.hpp"
#include "licht/renderer/render_context.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/texture.hpp"
#include "material_graphics_pipeline.hpp"
#include "licht/scene/punctual_light.hpp"
#include "ui_graphics_pipeline.hpp"

namespace licht {

class Camera;

class RenderFrameScript {
public:
    void on_startup();

    void on_shutdown();

    void on_tick(const float64 delta_time);

    void update_resized(const uint32 width, const uint32 height);

    bool compile_shaders();

    void reload_shaders();

    void pause();

    void unpause();

private:
    void update_material_uniform(const float64 delta_time);
    void update_ui_uniform(const float64 delta_time);

    void reset();

public:
    RenderFrameScript(Camera* camera, const WindowHandle window_handle);
    ~RenderFrameScript() = default;

private:
    Camera* camera_;

    DrawPacket packet_;
    DrawPacket ui_packet_;
    PunctualLight punctual_light_;

    WindowHandle window_handle_;
    SharedRef<RHIDevice> device_;
    
    Array<RHIFramebuffer*> material_framebuffers_;
    Array<RHIFramebuffer*> ui_framebuffers_;

    RHITextureView* depth_texture_view_ ;
    RHITexture* depth_texture_;

    SharedRef<RenderContext> render_context_;
    SharedRef<MaterialGraphicsPipeline> material_graphics_pipeline_;
    SharedRef<UIGraphicsPipeline> ui_graphics_pipeline_;

    bool pause_ = false;
};

}  //namespace licht