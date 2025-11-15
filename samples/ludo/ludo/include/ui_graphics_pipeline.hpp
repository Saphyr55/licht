#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/renderer/draw_item.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "ludo_types.hpp"

namespace licht {

class RenderContext;

class UIGraphicsPipeline {
public:
    void initialize(const SharedRef<RHIDevice>& device, const SharedRef<RenderContext>& render_context);

    void initialize_shader_resource_pool(size_t item_count);

    void reload();
    void destroy();

    void compile(DrawPacket& packet);

    void update(const UniformBufferObject& ubo);

    inline RHIRenderPass* get_render_pass() {
        return render_pass_;
    }

    inline RHIGraphicsPipeline* get_graphics_pipeline_handle() {
        return graphics_pipeline_;
    }

private:
    void create_pipeline_internal();
    void destroy_pipeline_internal();

private:
    RHIGraphicsPipeline* graphics_pipeline_ = nullptr;
    RHIRenderPass* render_pass_;
    
    Array<RHIShaderResourceBinding> global_bindings_;
    RHIShaderResourceGroupLayout* global_shader_resource_layout_ = nullptr ;
    RHIShaderResourceGroupPool* global_shader_resource_pool_ = nullptr;

    Array<RHIBuffer*> uniform_buffers_;
    SharedRef<RHIDevice> device_;
    SharedRef<RenderContext> render_context_;
};

}