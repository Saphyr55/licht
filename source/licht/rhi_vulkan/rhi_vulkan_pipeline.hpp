#pragma once

#include <vulkan/vulkan_core.h>

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/pipeline/pipeline.hpp"
#include "licht/rhi_vulkan/vulkan_shader_module.hpp"

namespace licht {

struct VulkanContext;

class RHIVulkanPipeline : public RHIPipeline {
public:
    void initialize();

    void destroy();

    VkPipeline pipeline() const { return pipeline_; };

    VkPipelineLayout layout() const { return pipeline_layout_; };

public:
    RHIVulkanPipeline(VulkanContext& context, const RHIPipelineDescription& description);

private:
    VulkanContext& context_;
    VkPipeline pipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
    RHIPipelineDescription description_;
};

using RHIVulkanPipelineRef = SharedRef<RHIVulkanPipeline>;


}  //namespace licht