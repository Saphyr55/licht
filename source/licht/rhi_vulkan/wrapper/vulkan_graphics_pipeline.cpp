#include "vulkan_graphics_pipeline.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_context.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

VulkanGraphicsPipeline::VulkanGraphicsPipeline(StringRef p_name, const VulkanShaderModule& p_vertex_shader_module, const VulkanShaderModule& p_fragment_shader_module)
    : name_(p_name)
    , vertex_shader_module_(p_vertex_shader_module)
    , fragment_shader_module_(p_fragment_shader_module) {
}

void VulkanGraphicsPipeline::init(VulkanContext* p_context) {
    LCHECK(p_context)

    vertex_shader_module_.init(p_context);
    fragment_shader_module_.init(p_context);

    VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {};
    vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_create_info.module = vertex_shader_module_.handle();
    vertex_shader_stage_create_info.pName = name_;

    VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {};
    fragment_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_stage_create_info.module = fragment_shader_module_.handle();
    fragment_shader_stage_create_info.pName = name_;

    VkPipelineShaderStageCreateInfo pipeline_shader_state_create_info[] = {vertex_shader_stage_create_info, fragment_shader_stage_create_info};

    Array<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo pipeline_dymanic_state_create_info = {};
    pipeline_dymanic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipeline_dymanic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    pipeline_dymanic_state_create_info.pDynamicStates = dynamic_states.data();

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
    vertex_input_state_create_info.pVertexBindingDescriptions = nullptr;  // Optional
    vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
    vertex_input_state_create_info.pVertexAttributeDescriptions = nullptr;  // Optional

    VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info{};
    pipeline_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipeline_input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipeline_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float32>(p_context->swapchain_extent.width);
    viewport.height = static_cast<float32>(p_context->swapchain_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = p_context->swapchain_extent;

    VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info = {};
    pipeline_viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipeline_viewport_state_create_info.viewportCount = 1;
    pipeline_viewport_state_create_info.pViewports = &viewport;
    pipeline_viewport_state_create_info.scissorCount = 1;
    pipeline_viewport_state_create_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo pipeline_rasterizer_state_create_info = {};
    pipeline_rasterizer_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipeline_rasterizer_state_create_info.depthClampEnable = VK_FALSE;
    pipeline_rasterizer_state_create_info.rasterizerDiscardEnable = VK_FALSE;
    pipeline_rasterizer_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    pipeline_rasterizer_state_create_info.lineWidth = 1.0f;
    pipeline_rasterizer_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
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

    VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state = {};
    pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pipeline_color_blend_attachment_state.blendEnable = VK_FALSE;
    pipeline_color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    pipeline_color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    pipeline_color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    pipeline_color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    pipeline_color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    pipeline_color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info{};
    pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipeline_color_blend_state_create_info.logicOpEnable = VK_FALSE;
    pipeline_color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
    pipeline_color_blend_state_create_info.attachmentCount = 1;
    pipeline_color_blend_state_create_info.pAttachments = &pipeline_color_blend_attachment_state;
    pipeline_color_blend_state_create_info.blendConstants[0] = 0.0f;  // Optional
    pipeline_color_blend_state_create_info.blendConstants[1] = 0.0f;  // Optional
    pipeline_color_blend_state_create_info.blendConstants[2] = 0.0f;  // Optional
    pipeline_color_blend_state_create_info.blendConstants[3] = 0.0f;  // Optional

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = 0;             // Optional
    pipeline_layout_create_info.pSetLayouts = nullptr;          // Optional
    pipeline_layout_create_info.pushConstantRangeCount = 0;     // Optional
    pipeline_layout_create_info.pPushConstantRanges = nullptr;  // Optional

    LICHT_VULKAN_CHECK(p_context->api.licht_vkCreatePipelineLayout(p_context->device, &pipeline_layout_create_info, p_context->allocator, &pipeline_layout_));

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
    graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_create_info.stageCount = 2;
    graphics_pipeline_create_info.pStages = pipeline_shader_state_create_info;
    graphics_pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_create_info.pInputAssemblyState = &pipeline_input_assembly_state_create_info;
    graphics_pipeline_create_info.pViewportState = &pipeline_viewport_state_create_info;
    graphics_pipeline_create_info.pRasterizationState = &pipeline_rasterizer_state_create_info;
    graphics_pipeline_create_info.pMultisampleState = &pipeline_multisampling_state_create_info;
    graphics_pipeline_create_info.pDepthStencilState = nullptr;  // Optional
    graphics_pipeline_create_info.pColorBlendState = &pipeline_color_blend_state_create_info;
    graphics_pipeline_create_info.pDynamicState = &pipeline_dymanic_state_create_info;
    graphics_pipeline_create_info.layout = pipeline_layout_;
    graphics_pipeline_create_info.renderPass = p_context->render_pass;
    graphics_pipeline_create_info.subpass = 0;
    graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;  // Optional
    graphics_pipeline_create_info.basePipelineIndex = -1;               // Optional

    LICHT_VULKAN_CHECK(p_context->api.licht_vkCreateGraphicsPipelines(p_context->device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, p_context->allocator, &graphics_pipeline_));

    vertex_shader_module_.destroy(p_context);
    fragment_shader_module_.destroy(p_context);
}

void VulkanGraphicsPipeline::destroy(VulkanContext* p_context) {
    LCHECK(p_context)

    p_context->api.licht_vkDestroyPipeline(p_context->device, graphics_pipeline_, p_context->allocator);
    p_context->api.licht_vkDestroyPipelineLayout(p_context->device, pipeline_layout_, p_context->allocator);
}

void vulkan_pipeline_bind(VulkanContext* p_context, VkCommandBuffer p_command_buffer) {
    p_context->api.licht_vkCmdBindPipeline(p_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p_context->graphics_pipeline->pipeline());
}

}  //namespace licht