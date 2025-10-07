#pragma once

#include "licht/core/string/string_ref.hpp"
#include "licht/rhi/compiled_shader.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/shader_resource.hpp"

namespace licht {

/**
 * @brief Describes a vertex buffer binding for the graphics pipeline.
 *
 * This structure specifies the binding index, stride, and input rate for a vertex buffer.
 */
struct RHIVertexBindingDescription {
    /**
     * @brief The binding index for the vertex buffer.
     */
    uint32 binding = 0;

    /**
     * @brief The stride (in bytes) between elements in the vertex buffer.
     */
    uint32 stride = 0;

    /**
     * @brief The input rate for the vertex data (per-vertex or per-instance).
     */
    RHIVertexInputRate input_rate = RHIVertexInputRate::Vertex;
};

/**
 * @struct RHIVertexAttributeDescription
 * @brief Describes a single vertex attribute for the RHI (Rendering Hardware Interface).
 *
 * This struct specifies the layout of a vertex attribute, including its binding index,
 * shader location, data format, and byte offset within the vertex buffer.
 */
struct RHIVertexAttributeDescription {
    /**
     * @brief The binding index of the vertex buffer.
     * Specifies which vertex buffer this attribute is sourced from.
     */
    uint32 binding = 0;

    /**
     * @brief The location of the attribute in the shader.
     * Corresponds to the input location in the vertex shader.
     */
    uint32 location = 0;

    /**
     * @brief The data format of the attribute.
     * Specifies how the attribute data is interpreted (e.g., float, int).
     */
    RHIFormat format = RHIFormat::R32Float;

    /**
     * @brief The byte offset of the attribute within the vertex buffer.
     * Indicates where the attribute data begins in the buffer.
     */
    uint32 offset = 0;
};

struct RHIGraphicsPipelineShaderStageCreateInfo {
    StringRef name = "";
    RHIShaderStage type = RHIShaderStage::AllGraphics;
    SPIRVShader shader;
};

struct RHIGraphicsPipelineViewportStateInformation {
    Viewport viewport;
    Rect2D scissor;
};

struct RHIGraphicsPipelineVertexBindingInformation {
    Array<RHIVertexBindingDescription> bindings;
    Array<RHIVertexAttributeDescription> attributes;
};

struct RHIGraphicsPipelineDescription {
    RHICullModeFlags cull_mode = RHICullModeFlags::None;
    RHIGraphicsPipelineShaderStageCreateInfo vertex_shader_info = {};
    RHIGraphicsPipelineShaderStageCreateInfo fragment_shader_info = {};
    RHIGraphicsPipelineViewportStateInformation viewport_info = {};
    RHIGraphicsPipelineVertexBindingInformation vertex_binding_info = {};
    RHIShaderResourceGroupLayout* shader_resource_group_layout = nullptr;
    RHIRenderPass* render_pass = nullptr;
};

class RHIGraphicsPipeline {
public:
    virtual ~RHIGraphicsPipeline() = default;
};

}  //namespace licht
