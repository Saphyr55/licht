#pragma once

#include "licht/core/string/string_ref.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "licht/rhi/compiled_shader.hpp"
#include "licht/rhi/rhi_types.hpp"

namespace licht {

struct RHIPipelineShaderStageCreateInfo {
    StringRef name;
    SPIRVShader* shader;
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
    Array<RHIShaderResourceBinding> bindings;
    RHIRenderPass* render_pass;
};

class RHIPipeline {
public:
    virtual ~RHIPipeline() = default;
};


}
