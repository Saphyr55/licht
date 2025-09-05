#pragma once

#include <vulkan/vulkan_core.h>

#include "licht/rhi/pipeline/graphics_pipeline.hpp"
#include "licht/rhi_vulkan/vulkan_shader_module.hpp"

namespace licht {

struct VulkanContext;

class RHIVulkanGraphicsPipeline {
public:
    void initialize();

    void destroy();

    void bind(VkCommandBuffer command_buffer);

    VkPipeline pipeline() const { return pipeline_; };

    VkPipelineLayout layout() const { return pipeline_layout_; };

public:
    RHIVulkanGraphicsPipeline(VulkanContext& context, const RHIPipelineDescription& description);

private:
    VulkanContext& context_;
    VkPipeline pipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
    RHIPipelineDescription description_;
};


}  //namespace licht