#pragma once

#include <vulkan/vulkan_core.h>

#include "licht/rhi/graphics_pipeline.hpp"

namespace licht {

struct VulkanContext;

class VulkanGraphicsPipeline : public RHIGraphicsPipeline {
public:
    void initialize(const RHIGraphicsPipelineDescription& description);

    void destroy();

    VkPipeline& get_handle() { return pipeline_; };

    VkPipelineLayout& get_layout() { return pipeline_layout_; };

public:
    VulkanGraphicsPipeline();

private:
    VkPipeline pipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
    RHIGraphicsPipelineDescription description_;
};

}  //namespace licht
