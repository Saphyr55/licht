#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/renderer/render_context.hpp"
#include "licht/renderer/draw_item.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "ludo_types.hpp"

namespace licht {

struct PunctualLight;

class MaterialGraphicsPipeline {
public:
    void initialize(const SharedRef<RHIDevice>& device,
                    const SharedRef<RenderContext>& renderer);

    void initialize_shader_resource_pool(size_t item_count);
    
    void reload();
    void destroy();

    void compile(const DrawPacket& packet);

    void update(const PunctualLight& light);
    void update(const UniformBufferObject& ubo);

    inline RHIGraphicsPipeline* get_graphics_pipeline_handle() {
        return graphics_pipeline_;
    }

    inline RHIRenderPass* get_render_pass() {
        return render_pass_;
    }
    
    inline RHIShaderResourceGroupLayout* get_global_shader_resource_layout() {
        return global_shader_resource_layout_;
    }

    inline RHIShaderResourceGroupPool* get_global_shader_resource_pool() {
        return global_shader_resource_pool_;
    }

    MaterialGraphicsPipeline() = default;
    ~MaterialGraphicsPipeline() = default;

private:
    void destroy_pipeline_internal();
    void create_pipeline_internal();

private:
    SharedRef<RHIDevice> device_ = nullptr;
    SharedRef<RenderContext> render_context_ = nullptr;
    SharedRef<RHIBufferPool> buffer_pool_ = nullptr;
    SharedRef<RHITexturePool> texture_pool_ = nullptr;
    RHIGraphicsPipeline* graphics_pipeline_ = nullptr;
    RHIRenderPass* render_pass_ = nullptr;

    Array<RHIShaderResourceBinding> global_bindings_;
    RHIShaderResourceGroupLayout* global_shader_resource_layout_ = nullptr ;
    RHIShaderResourceGroupPool* global_shader_resource_pool_ = nullptr;

    Array<RHIBuffer*> uniform_buffers_;
    Array<RHIBuffer*> light_buffers_;
    Array<RHIBuffer*> sampler_indices_buffers_;

    static constexpr const uint32 sampler_count = 2;
    struct SamplerIndices {
        int32 diffuse_index = 0;
        int32 normal_index = 1;
        int32 unused1 = -1;
        int32 unused2 = -1;
    } sampler_indices_;

    Array<RHIShaderResourceBinding> texture_bindings_;
    RHIShaderResourceGroupLayout* texture_shader_resource_layout_ = nullptr;
    RHIShaderResourceGroupPool* texture_shader_resource_pool_ = nullptr;
    Array<RHIBuffer*> texture_factors_buffers_;

    RHITexture* default_texture_ = nullptr;
    RHITextureView* default_texture_view_ = nullptr;
    RHISampler* default_sampler_ = nullptr;
};

}  //namespace licht