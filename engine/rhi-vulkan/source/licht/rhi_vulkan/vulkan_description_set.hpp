#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/memory/linear_allocator.hpp"
#include "licht/rhi/shader_resource.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class VulkanDescriptorSet : public RHIShaderResource {
public:
    virtual void update(RHIBuffer* buffer, size_t binding, size_t offset, size_t range) override;

    inline VkDescriptorSet get_handle() {
        return descriptor_set_;
    }

    VulkanDescriptorSet(VulkanContext& context, VkDescriptorSet descriptor_set)
        : context_(context), descriptor_set_(descriptor_set) {}

    virtual ~VulkanDescriptorSet() override = default;

private:
    VulkanContext& context_;
    VkDescriptorSet descriptor_set_;
};

class VulkanDescriptorPool : public RHIShaderResourcePool {
public:
    virtual RHIShaderResource* get_shader_resource(uint32 index) override;

    void initialize();

    void destroy();

    VulkanDescriptorPool(VulkanContext& context, VkDescriptorSetLayout layout, size_t count);

    virtual ~VulkanDescriptorPool() override;

private:
    VulkanContext& context_;
    VkDescriptorSetLayout layout_;
    VkDescriptorPool descriptor_pool_;
    Array<VulkanDescriptorSet> descriptor_sets_;
};

}  //namespace licht
