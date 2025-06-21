#pragma once

#include <vulkan/vulkan_core.h>
#include "licht/core/string/string_ref.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_shader_module.hpp"

namespace licht {

struct VulkanContext;

class VulkanGraphicsPipeline {
public:
    void init(VulkanContext* p_context);

    void destroy(VulkanContext* p_context);
    
    VkPipeline pipeline() const { return graphics_pipeline_; };

    VkPipelineLayout layout() const { return pipeline_layout_; };

    VulkanGraphicsPipeline(StringRef p_name, const VulkanShaderModule& p_vertex_shader_module, const VulkanShaderModule& p_fragment_shader_module);

private:
    StringRef name_;
    VkPipeline graphics_pipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
    VulkanShaderModule vertex_shader_module_;
    VulkanShaderModule fragment_shader_module_;
};

void vulkan_pipeline_bind(VulkanContext* p_context, VkCommandBuffer p_command_buffer);

}