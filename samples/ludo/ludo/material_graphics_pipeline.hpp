#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/renderer/renderer.hpp"
#include "licht/renderer/render_item.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "ludo_types.hpp"

namespace licht {

class MaterialGraphicsPipeline {
public:
    void initiliaze(const SharedRef<RHIDevice>& device,
                    const SharedRef<Renderer>& renderer,
                    const SharedRef<RHIBufferPool>& buffer_pool);

    void destroy();

    void compile(const Array<RenderPacket>& packets);

    void update(const UniformBufferObject& ubo);

    inline RHIGraphicsPipeline* get_graphics_pipeline_handle() {
        return graphics_pipeline_;
    }

    inline RHIRenderPass* get_render_pass() {
        return render_pass_;
    }

    inline RHIShaderResourceGroupLayout* get_shader_resource_layout() {
        return shader_resource_layout_;
    }

    inline RHIShaderResourceGroupPool* get_shader_resource_pool() {
        return shader_resource_pool_;
    }

    MaterialGraphicsPipeline() = default;
    ~MaterialGraphicsPipeline() = default;

private:
    SharedRef<RHIDevice> device_;
    SharedRef<Renderer> renderer_;
    SharedRef<RHIBufferPool> buffer_pool_;
    RHIRenderPass* render_pass_;
    RHIShaderResourceBinding ubo_binding_;
    RHIShaderResourceBinding sampler_binding_;
    RHIGraphicsPipeline* graphics_pipeline_;
    RHIShaderResourceGroupLayout* shader_resource_layout_;
    RHIShaderResourceGroupPool* shader_resource_pool_;
    Array<RHIBuffer*> uniform_buffers_;
};

}  //namespace licht