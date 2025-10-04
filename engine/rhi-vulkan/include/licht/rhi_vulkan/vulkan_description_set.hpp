#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/memory/linear_allocator.hpp"
#include "licht/rhi/descriptor_set.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class RHIVulkanDescriptorSet : public RHIDescriptorSet {
public:
    virtual void update(RHIBufferRef buffer, size_t binding, size_t offset, size_t range) override;

    inline VkDescriptorSet get_handle() {
        return descriptor_set_;
    }

    RHIVulkanDescriptorSet(VulkanContext& context, VkDescriptorSet descriptor_set)
        : context_(context), descriptor_set_(descriptor_set) {}

    virtual ~RHIVulkanDescriptorSet() override = default;

private:
    VulkanContext& context_;
    VkDescriptorSet descriptor_set_;
};

using RHIVulkanDescriptorSetRef = SharedRef<RHIVulkanDescriptorSet>;

class RHIVulkanDescriptorPool : public RHIDescriptorPool {
public:
    virtual RHIDescriptorSetRef get_descriptor_set(uint32 index) override;

    void initialize();

    void destroy();

    RHIVulkanDescriptorPool(VulkanContext& context,
                            VkDescriptorSetLayout layout,
                            const RHIDescriptorSetInformation& information);

    virtual ~RHIVulkanDescriptorPool() override;

private:
    VulkanContext& context_;
    VkDescriptorSetLayout layout_;
    RHIDescriptorSetInformation information_;
    VkDescriptorPool descriptor_pool_;
    Array<RHIVulkanDescriptorSetRef> descriptor_sets_;
};

using RHIVulkanDescriptorPoolRef = SharedRef<RHIVulkanDescriptorPool>;

}  //namespace licht
