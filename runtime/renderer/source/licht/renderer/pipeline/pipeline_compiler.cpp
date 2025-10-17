#include "licht/renderer/pipeline/pipeline_compiler.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi/device.hpp"

namespace licht {

PipelineCompiler::PipelineCompiler(const SharedRef<RHIDevice>& device)
    : device_(device) {
    LCHECK(device);
}

SharedRef<RHIGraphicsPipeline> PipelineCompiler::compile_graphics(const GraphicsPipelineConfig& cfg) {
    LCHECK(cfg.render_pass != nullptr);

    auto desc_builder = RHIGraphicsPipelineDescriptionBuilder()
                            .with_render_pass(cfg.render_pass)
                            .with_vertex_shader(cfg.vertex_shader, cfg.vertex_entry)
                            .with_fragment_shader(cfg.fragment_shader, cfg.fragment_entry)
                            .with_vertex_bindings(cfg.vertex_bindings, cfg.vertex_attributes)
                            .with_shader_resource_group_layout(cfg.shader_resource_layouts)
                            .with_shader_constant_ranges(cfg.shader_constant_ranges)
                            .with_cull_mode(cfg.cull_mode)
                            .with_viewport(cfg.viewport, cfg.scissor)
                            // .with_topology(cfg.topology)
                            ;

    return device_->create_graphics_pipeline(desc_builder.build());
}

}  //namespace licht