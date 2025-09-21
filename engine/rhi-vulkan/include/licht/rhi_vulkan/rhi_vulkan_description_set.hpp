#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/rhi/descriptor_set.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanDescriptorSetLayout : public RHIDescriptorSetLayout {
public:
    inline VkDescriptorSetLayout get_handle() {
        return handle_;
    }

    void initialize();

    void destroy();

    RHIVulkanDescriptorSetLayout(VulkanContext& context, const Array<RHIDescriptorSetLayoutBinding>& bindings);
    virtual ~RHIVulkanDescriptorSetLayout() override;

private:
    VulkanContext& context_;
    Array<RHIDescriptorSetLayoutBinding> bindings_;
    VkDescriptorSetLayout handle_;
};

using RHIVulkanDescriptorSetLayoutRef = SharedRef<RHIVulkanDescriptorSetLayout>;

}