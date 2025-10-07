#include "licht/rhi_vulkan/vulkan_graphics_pipeline.hpp"

#include "licht/core/defines.hpp"
#include "licht/rhi/compiled_shader.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_render_pass.hpp"
#include "vulkan_description_set.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

static constexpr auto vk_binding_map = [](const RHIVertexBindingDescription& description) -> VkVertexInputBindingDescription {
    VkVertexInputBindingDescription vulkan_description;
    vulkan_description.binding = description.binding;
    vulkan_description.stride = description.stride;
    vulkan_description.inputRate = vulkan_vertex_input_rate_get(description.input_rate);
    return vulkan_description;
};

static constexpr auto vk_attribute_map = [](const RHIVertexAttributeDescription& description) -> VkVertexInputAttributeDescription {
    VkVertexInputAttributeDescription vulkan_description;
    vulkan_description.binding = description.binding;
    vulkan_description.location = description.location;
    vulkan_description.format = vulkan_format_get(description.format);
    vulkan_description.offset = description.offset;
    return vulkan_description;
};

VulkanGraphicsPipeline::VulkanGraphicsPipeline() = default;

void VulkanGraphicsPipeline::initialize(const RHIGraphicsPipelineDescription& description) {
    VulkanContext& context = vulkan_context_get();
    description_ = description;

    SPIRVShader& vertex_compiled_shader = description_.vertex_shader_info.shader;
    VulkanShaderModule vertex_shader_module(vertex_compiled_shader.get_bytes());

    SPIRVShader& fragment_compiled_shader = description_.fragment_shader_info.shader;
    VulkanShaderModule fragment_shader_module(fragment_compiled_shader.get_bytes());

    VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info{};
    pipeline_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipeline_input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipeline_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

    vertex_shader_module.initialize();
    fragment_shader_module.initialize();

    Array<VkVertexInputBindingDescription> vertex_bindings = description_.vertex_binding_info.bindings
                                                                 .map<VkVertexInputBindingDescription>(vk_binding_map);

    Array<VkVertexInputAttributeDescription> vertex_attributes = description_.vertex_binding_info.attributes
                                                                     .map<VkVertexInputAttributeDescription>(vk_attribute_map);

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.vertexBindingDescriptionCount = vertex_bindings.size();
    vertex_input_state_create_info.pVertexBindingDescriptions = vertex_bindings.data();
    vertex_input_state_create_info.vertexAttributeDescriptionCount = vertex_attributes.size();
    vertex_input_state_create_info.pVertexAttributeDescriptions = vertex_attributes.data();

    VkPipelineRasterizationStateCreateInfo pipeline_rasterizer_state_create_info = {};
    pipeline_rasterizer_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipeline_rasterizer_state_create_info.depthClampEnable = VK_FALSE;
    pipeline_rasterizer_state_create_info.rasterizerDiscardEnable = VK_FALSE;
    pipeline_rasterizer_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    pipeline_rasterizer_state_create_info.lineWidth = 1.0f;

    VkCullModeFlags cull_mode = VK_CULL_MODE_NONE;
    if (description_.cull_mode == RHICullModeFlags::Back) {
        cull_mode = VK_CULL_MODE_BACK_BIT;
    } else if (description_.cull_mode == RHICullModeFlags::Front) {
        cull_mode = VK_CULL_MODE_FRONT_BIT;
    } else if (description_.cull_mode == RHICullModeFlags::None) {
        cull_mode = VK_CULL_MODE_NONE;
    }

    pipeline_rasterizer_state_create_info.cullMode = cull_mode;
    // pipeline_rasterizer_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pipeline_rasterizer_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    pipeline_rasterizer_state_create_info.depthBiasEnable = VK_FALSE;
    pipeline_rasterizer_state_create_info.depthBiasConstantFactor = 0.0f;  // Optional
    pipeline_rasterizer_state_create_info.depthBiasClamp = 0.0f;           // Optional
    pipeline_rasterizer_state_create_info.depthBiasSlopeFactor = 0.0f;     // Optional

    VkPipelineMultisampleStateCreateInfo pipeline_multisampling_state_create_info = {};
    pipeline_multisampling_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipeline_multisampling_state_create_info.sampleShadingEnable = VK_FALSE;
    pipeline_multisampling_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pipeline_multisampling_state_create_info.minSampleShading = 1.0f;           // Optional
    pipeline_multisampling_state_create_info.pSampleMask = nullptr;             // Optional
    pipeline_multisampling_state_create_info.alphaToCoverageEnable = VK_FALSE;  // Optional
    pipeline_multisampling_state_create_info.alphaToOneEnable = VK_FALSE;       // Optional

    VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {};
    vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_create_info.module = vertex_shader_module.handle();
    vertex_shader_stage_create_info.pName = description_.vertex_shader_info.name;

    VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {};
    fragment_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_stage_create_info.module = fragment_shader_module.handle();
    fragment_shader_stage_create_info.pName = description_.fragment_shader_info.name;

    VkPipelineShaderStageCreateInfo pipeline_shader_state_create_info[] = {vertex_shader_stage_create_info, fragment_shader_stage_create_info};

    Array<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_LINE_WIDTH,
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo pipeline_dymanic_state_create_info = {};
    pipeline_dymanic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipeline_dymanic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    pipeline_dymanic_state_create_info.pDynamicStates = dynamic_states.data();

    VkViewport viewport = {};
    viewport.x = description_.viewport_info.viewport.x;
    viewport.y = description_.viewport_info.viewport.y;
    viewport.width = description_.viewport_info.viewport.width;
    viewport.height = description_.viewport_info.viewport.height;
    viewport.minDepth = description_.viewport_info.viewport.min_depth;
    viewport.maxDepth = description_.viewport_info.viewport.max_depth;

    int32 offset_x = description_.viewport_info.scissor.x;
    int32 offset_y = description_.viewport_info.scissor.y;
    VkRect2D scissor = {};
    scissor.offset = {offset_x, offset_y};
    scissor.extent = {
        static_cast<uint32>(description_.viewport_info.scissor.width),
        static_cast<uint32>(description_.viewport_info.scissor.height),
    };

    VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info = {};
    pipeline_viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipeline_viewport_state_create_info.viewportCount = 1;
    pipeline_viewport_state_create_info.pViewports = &viewport;
    pipeline_viewport_state_create_info.scissorCount = 1;
    pipeline_viewport_state_create_info.pScissors = &scissor;

    VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state = {};
    pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pipeline_color_blend_attachment_state.blendEnable = VK_FALSE;
    pipeline_color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    pipeline_color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    pipeline_color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    pipeline_color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    pipeline_color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    pipeline_color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info = {};
    pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipeline_color_blend_state_create_info.logicOpEnable = VK_FALSE;
    pipeline_color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
    pipeline_color_blend_state_create_info.attachmentCount = 1;
    pipeline_color_blend_state_create_info.pAttachments = &pipeline_color_blend_attachment_state;
    pipeline_color_blend_state_create_info.blendConstants[0] = 0.0f;  // Optional
    pipeline_color_blend_state_create_info.blendConstants[1] = 0.0f;  // Optional
    pipeline_color_blend_state_create_info.blendConstants[2] = 0.0f;  // Optional
    pipeline_color_blend_state_create_info.blendConstants[3] = 0.0f;  // Optional

    VulkanShaderResourceGroupLayout* layout = static_cast<VulkanShaderResourceGroupLayout*>(description_.shader_resource_group_layout);
    LCHECK(layout)
    
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = 1;
    pipeline_layout_create_info.pSetLayouts = &layout->get_handle();
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = nullptr;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreatePipelineLayout(context.device, &pipeline_layout_create_info, context.allocator, &pipeline_layout_));

    VulkanRenderPass* render_pass = static_cast<VulkanRenderPass*>(description_.render_pass);

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
    graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_create_info.stageCount = 2;  // TODO: Configurable
    graphics_pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_create_info.pInputAssemblyState = &pipeline_input_assembly_state_create_info;
    graphics_pipeline_create_info.pStages = pipeline_shader_state_create_info;
    graphics_pipeline_create_info.pViewportState = &pipeline_viewport_state_create_info;
    graphics_pipeline_create_info.pRasterizationState = &pipeline_rasterizer_state_create_info;
    graphics_pipeline_create_info.pMultisampleState = &pipeline_multisampling_state_create_info;
    graphics_pipeline_create_info.pDepthStencilState = nullptr;  // TODO: Configurable
    graphics_pipeline_create_info.pColorBlendState = &pipeline_color_blend_state_create_info;
    graphics_pipeline_create_info.pDynamicState = &pipeline_dymanic_state_create_info;
    graphics_pipeline_create_info.layout = pipeline_layout_;
    graphics_pipeline_create_info.renderPass = render_pass->get_handle();
    graphics_pipeline_create_info.subpass = 0;                          // TODO: Configurable
    graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;  // TODO: Configurable
    graphics_pipeline_create_info.basePipelineIndex = -1;               // TODO: Configurable

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateGraphicsPipelines(context.device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, context.allocator, &pipeline_));

    vertex_shader_module.destroy();
    fragment_shader_module.destroy();
}

void VulkanGraphicsPipeline::destroy() {
    VulkanContext& context = vulkan_context_get();
    VulkanShaderResourceGroupLayout* layout = static_cast<VulkanShaderResourceGroupLayout*>(description_.shader_resource_group_layout);

    VulkanAPI::lvkDestroyPipeline(context.device, pipeline_, context.allocator);
    VulkanAPI::lvkDestroyPipelineLayout(context.device, pipeline_layout_, context.allocator);
    VulkanAPI::lvkDestroyDescriptorSetLayout(context.device, layout->get_handle(), context.allocator);
}

}  //namespace licht