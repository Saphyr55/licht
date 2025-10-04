#pragma once

#include "licht/core/string/string_ref.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/descriptor_set.hpp"
#include "licht/rhi/pipeline/compiled_shader.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

struct RHIPipelineShaderStageCreateInfo {
    StringRef name;
    CompiledShader* shader;
    RHIShaderStage type;
};

struct RHIPipelineViewportStateInformation {
    Viewport viewport;
    Rect2D scissor; 
};

struct RHIPipelineVertexBindingInformation {
    Array<RHIVertexBindingDescription> bindings;
    Array<RHIVertexAttributeDescription> attributes;
};

struct RHIPipelineDescription {
    RHIPipelineShaderStageCreateInfo vertex_shader_info;
    RHIPipelineShaderStageCreateInfo fragment_shader_info;
    RHIPipelineViewportStateInformation viewport_info;
    RHIPipelineVertexBindingInformation vertex_binding_info;
    Array<RHIDescriptorSetLayoutBinding> bindings;
    RHIRenderPassRef render_pass;
};

class RHIPipeline {
public:
    virtual ~RHIPipeline() = default;
};


}
