#pragma once

#include "licht/core/string/string_ref.hpp"
#include "licht/rhi/compiled_shader.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "pipeline.hpp"

namespace licht {

struct RHIVertexBindingDescription {
    uint32 binding = 0;
    uint32 stride = 0;
    RHIVertexInputRate input_rate = RHIVertexInputRate::Vertex;

    RHIVertexBindingDescription() = default;
    RHIVertexBindingDescription(uint32 in_binding,
                                uint32 in_stride,
                                RHIVertexInputRate in_input_rate = RHIVertexInputRate::Vertex)
        : binding(in_binding)
        , stride(in_stride)
        , input_rate(in_input_rate) {}
};

struct RHIVertexAttributeDescription {
    uint32 binding = 0;
    uint32 location = 0;
    RHIFormat format = RHIFormat::R32Float;
    uint32 offset = 0;

    RHIVertexAttributeDescription() = default;
    RHIVertexAttributeDescription(uint32 in_binding,
                                  uint32 in_location,
                                  RHIFormat in_format,
                                  uint32 in_offset = 0)
        : binding(in_binding)
        , location(in_location)
        , format(in_format)
        , offset(in_offset) {}
};

struct RHIGraphicsPipelineShaderStageCreateInfo {
    StringRef name = "main";
    SPIRVShader shader;
    RHIShaderStage type = RHIShaderStage::AllGraphics;
};

struct RHIGraphicsPipelineViewportStateInformation {
    Viewport viewport;
    Rect2D scissor;
};

struct RHIGraphicsPipelineVertexBindingInformation {
    Array<RHIVertexBindingDescription> bindings;
    Array<RHIVertexAttributeDescription> attributes;
};

struct RHIShaderConstantRange {
    RHIShaderStage stage = RHIShaderStage::AllGraphics;
    uint32 offset = 0;
    uint32 size = 128;
};

struct RHIGraphicsPipelineDescription {
    RHICullModeFlags cull_mode = RHICullModeFlags::None;
    RHIGraphicsPipelineShaderStageCreateInfo vertex_shader_info = {};
    RHIGraphicsPipelineShaderStageCreateInfo fragment_shader_info = {};
    RHIGraphicsPipelineViewportStateInformation viewport_info = {};
    RHIGraphicsPipelineVertexBindingInformation vertex_binding_info = {};
    Array<RHIShaderResourceGroupLayout*> layouts;
    Array<RHIShaderConstantRange> push_constant_ranges;
    RHIRenderPass* render_pass = nullptr;
};

class RHIGraphicsPipelineDescriptionBuilder {
public:
    RHIGraphicsPipelineDescriptionBuilder& with_cull_mode(RHICullModeFlags mode) {
        desc_.cull_mode = mode;
        return *this;
    }

    RHIGraphicsPipelineDescriptionBuilder& with_vertex_shader(const SPIRVShader& shader, StringRef name = "main") {
        desc_.vertex_shader_info = {
            .name = name, 
            .shader = shader, 
            .type = RHIShaderStage::Vertex
        };
        return *this;
    }

    RHIGraphicsPipelineDescriptionBuilder& with_fragment_shader(const SPIRVShader& shader, StringRef name = "main") {
        desc_.fragment_shader_info = {
            .name = name, 
            .shader = shader, 
            .type = RHIShaderStage::Fragment
        };
        return *this;
    }

    RHIGraphicsPipelineDescriptionBuilder& with_viewport(const Viewport& viewport, const Rect2D& scissor) {
        desc_.viewport_info = {viewport, scissor};
        return *this;
    }

    RHIGraphicsPipelineDescriptionBuilder& with_vertex_bindings(
        const Array<RHIVertexBindingDescription>& bindings,
        const Array<RHIVertexAttributeDescription>& attributes) {
        desc_.vertex_binding_info = {bindings, attributes};
        return *this;
    }

    RHIGraphicsPipelineDescriptionBuilder& with_shader_resource_group_layout(const Array<RHIShaderResourceGroupLayout*>& layouts) {
        desc_.layouts = layouts;
        return *this;
    }
    
    RHIGraphicsPipelineDescriptionBuilder& with_shader_constant_ranges(const Array<RHIShaderConstantRange>& ranges) {
        desc_.push_constant_ranges = ranges;
        return *this;
    }

    RHIGraphicsPipelineDescriptionBuilder& with_render_pass(RHIRenderPass* render_pass) {
        desc_.render_pass = render_pass;
        return *this;
    }

    RHIGraphicsPipelineDescription build() const {
        return desc_;
    }

private:
    RHIGraphicsPipelineDescription desc_ = {};
};

class RHIGraphicsPipeline : public RHIPipeline {
public:
    virtual ~RHIGraphicsPipeline() = default;
};

}  //namespace licht
