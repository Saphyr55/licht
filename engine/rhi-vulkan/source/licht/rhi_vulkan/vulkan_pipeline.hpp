#pragma once

#include <vulkan/vulkan_core.h>

#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi/pipeline.hpp"
#include "licht/rhi_vulkan/vulkan_shader_module.hpp"

namespace licht {

struct VulkanContext;

class VulkanPipeline : public RHIPipeline {
public:
    void initialize();

    void destroy();

    VkDescriptorSetLayout get_descriptor_set_layout() { return descriptor_set_layout_; }

    VkPipeline& get_handle() { return pipeline_; };

    VkPipelineLayout& get_layout() { return pipeline_layout_; };

public:
    VulkanPipeline(VulkanContext& context, const RHIPipelineDescription& description);

private:
    VulkanContext& context_;
    VkPipeline pipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
    RHIPipelineDescription description_;
    VkDescriptorSetLayout descriptor_set_layout_;
};

using RHIVulkanPipelineRef = SharedRef<VulkanPipeline>;

}  //namespace licht