#include "rhi_vulkan_graphics_pipeline.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/io/file_handle.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/rhi/pipeline/compiled_shader.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

RHIVulkanGraphicsPipeline::RHIVulkanGraphicsPipeline(VulkanContext& context, const RHIPipelineDescription& description)
    : context_(context)
    , description_(description) {
}

void RHIVulkanGraphicsPipeline::initialize() {
    
    SharedRef<CompiledShader> vertex_compiled_shader = description_.vertex_shader_info.shader;
    VulkanShaderModule vertex_shader_module(vertex_compiled_shader->get_bytes());

    SharedRef<CompiledShader> fragment_compiled_shader = description_.fragment_shader_info.shader;
    VulkanShaderModule fragment_shader_module(fragment_compiled_shader->get_bytes());

    VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info{};
    pipeline_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipeline_input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipeline_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
    vertex_input_state_create_info.pVertexBindingDescriptions = nullptr;  // Optional
    vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
    vertex_input_state_create_info.pVertexAttributeDescriptions = nullptr;  // Optional

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

    vertex_shader_module.initialize(context_);
    fragment_shader_module.initialize(context_);

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
    scissor.offset = {offset_x, offset_y };
    scissor.extent = {
       static_cast<uint32>(description_.viewport_info.scissor.width),
       static_cast<uint32>(description_.viewport_info.scissor.height), 
    } ;

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

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreatePipelineLayout(context_.device, &pipeline_layout_create_info, context_.allocator, &pipeline_layout_));

    SharedRef<RHIVulkanRenderPass> render_pass = static_ref_cast<RHIVulkanRenderPass>(description_.render_pass);

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
    graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_create_info.stageCount = 2;
    graphics_pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_create_info.pInputAssemblyState = &pipeline_input_assembly_state_create_info;
    graphics_pipeline_create_info.pStages = pipeline_shader_state_create_info;
    graphics_pipeline_create_info.pViewportState = &pipeline_viewport_state_create_info;
    graphics_pipeline_create_info.pRasterizationState = &pipeline_rasterizer_state_create_info;
    graphics_pipeline_create_info.pMultisampleState = &pipeline_multisampling_state_create_info;
    graphics_pipeline_create_info.pDepthStencilState = nullptr;  // Optional
    graphics_pipeline_create_info.pColorBlendState = &pipeline_color_blend_state_create_info;
    graphics_pipeline_create_info.pDynamicState = &pipeline_dymanic_state_create_info;
    graphics_pipeline_create_info.layout = pipeline_layout_;
    graphics_pipeline_create_info.renderPass = render_pass->get_handle();
    graphics_pipeline_create_info.subpass = 0;
    graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;  // Optional
    graphics_pipeline_create_info.basePipelineIndex = -1;               // Optional

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateGraphicsPipelines(context_.device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, context_.allocator, &pipeline_));

    vertex_shader_module.destroy(context_);
    fragment_shader_module.destroy(context_);
}

void RHIVulkanGraphicsPipeline::destroy() {
    VulkanAPI::lvkDestroyPipeline(context_.device, pipeline_, context_.allocator);
    VulkanAPI::lvkDestroyPipelineLayout(context_.device, pipeline_layout_, context_.allocator);
}

/*
SharedRef<RHIVulkanGraphicsPipeline> RHIVulkanGraphicsPipeline::create(VulkanContext& context, StringRef p_name, const Array<uint8>& p_vertex_code, const Array<uint8>& p_fragment_code) {
    VulkanShaderModule vertex_shader_module(p_vertex_code);
    VulkanShaderModule fragment_shader_module(p_fragment_code);

    return new_ref<RHIVulkanGraphicsPipeline>(context, p_name, vertex_shader_module, fragment_shader_module);
}

SharedRef<RHIVulkanGraphicsPipeline> RHIVulkanGraphicsPipeline::create(VulkanContext& context, StringRef p_name, StringRef p_vertex_path, StringRef p_fragment_path) {
    FileHandleResult vertex_file_open_error = FileSystem::get_platform().open_read("shaders/main.vert.spv");
    LCHECK(vertex_file_open_error.has_value())
    SharedRef<FileHandle> vertex_file_handle = vertex_file_open_error.value();
    Array<uint8> vertex_code = vertex_file_handle->read_all_bytes();

    FileHandleResult fragment_file_open_error = FileSystem::get_platform().open_read("shaders/main.frag.spv");
    LCHECK(fragment_file_open_error.has_value())
    SharedRef<FileHandle> fragment_file_handle = fragment_file_open_error.value();
    Array<uint8> fragment_code = fragment_file_handle->read_all_bytes();

    return create(context, p_name, vertex_code, fragment_code);
}
*/

void RHIVulkanGraphicsPipeline::bind(VkCommandBuffer command_buffer) {
    VulkanAPI::lvkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
}

}  //namespace licht