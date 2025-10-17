#pragma once

#include "licht/renderer/renderer_exports.hpp"
#include "licht/rhi/graphics_pipeline.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

struct GraphicsPipelineConfig {
    // Shaders (must be supplied before compile)
    SPIRVShader vertex_shader;
    StringRef vertex_entry = "main";

    SPIRVShader fragment_shader;
    StringRef fragment_entry = "main";

    // Render pass this pipeline targets (required)
    RHIRenderPass* render_pass = nullptr;

    // Vertex input
    Array<RHIVertexBindingDescription> vertex_bindings;
    Array<RHIVertexAttributeDescription> vertex_attributes;

    // Shader resource layouts (set order matters)
    Array<RHIShaderResourceGroupLayout*> shader_resource_layouts;

    Viewport viewport;
    Rect2D scissor;

    // Shader constant ranges
    Array<RHIShaderConstantRange> shader_constant_ranges;

    // Raster state
    RHICullModeFlags cull_mode = RHICullModeFlags::Back;
    // TODO: Presice the topology
    // RHIPrimitiveTopology topology = RHIPrimitiveTopology::TriangleList;
};

class LICHT_RENDERER_API GraphicsPipelineConfigBuilder {
public:
    GraphicsPipelineConfigBuilder() = default;

    GraphicsPipelineConfigBuilder& with_vertex_shader_from_file(StringRef path, StringRef entry= "main");

    GraphicsPipelineConfigBuilder& with_vertex_shader(const SPIRVShader& shader, StringRef entry = "main") {
        cfg_.vertex_shader = shader;
        cfg_.vertex_entry = entry;
        return *this;
    }

    GraphicsPipelineConfigBuilder& with_fragment_shader_from_file(StringRef path, StringRef entry= "main");

    GraphicsPipelineConfigBuilder& with_fragment_shader(const SPIRVShader& shader, StringRef entry = "main") {
        cfg_.fragment_shader = shader;
        cfg_.fragment_entry = entry;
        return *this;
    }

    GraphicsPipelineConfigBuilder& with_render_pass(RHIRenderPass* render_pass) {
        cfg_.render_pass = render_pass;
        return *this;
    }

    GraphicsPipelineConfigBuilder& with_vertex_bindings(const Array<RHIVertexBindingDescription>& bindings,
                                                        const Array<RHIVertexAttributeDescription>& attributes) {
        cfg_.vertex_bindings = bindings;
        cfg_.vertex_attributes = attributes;
        return *this;
    }

    GraphicsPipelineConfigBuilder& with_shader_resource_layouts(const Array<RHIShaderResourceGroupLayout*>& layouts) {
        cfg_.shader_resource_layouts = layouts;
        return *this;
    }

    GraphicsPipelineConfigBuilder& with_viewport(const Viewport& vp, const Rect2D& scissor) {
        cfg_.viewport = vp;
        cfg_.scissor = scissor;
        return *this;
    }

    GraphicsPipelineConfigBuilder& with_shader_constant_ranges(const Array<RHIShaderConstantRange>& ranges) {
        cfg_.shader_constant_ranges = ranges;
        return *this;
    }

    GraphicsPipelineConfigBuilder& with_cull_mode(RHICullModeFlags cull) {
        cfg_.cull_mode = cull;
        return *this;
    }

    // TODO: With primitive topology in pipeline config builder.
    // PipelineConfigBuilder& with_topology(RHIPrimitiveTopology topo) {
    //     cfg_.topology = topo;
    //     return *this;
    // }

    GraphicsPipelineConfig build() const {
        return cfg_;
    }

private:
    GraphicsPipelineConfig cfg_;
};

}  //namespace licht