#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/containers/hash_set.hpp"
#include "licht/rhi/shader_resource.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

class VulkanShaderResourceGroupLayout : public RHIShaderResourceGroupLayout {
public:
    VkDescriptorSetLayout& get_handle() {
        return handle_;
    }

    virtual RHIShaderResourceType get_resource_type(size_t binding) const override;

    void initialize(const Array<RHIShaderResourceBinding>& bindings);
    void destroy();

public:
    VulkanShaderResourceGroupLayout() = default;
    ~VulkanShaderResourceGroupLayout() = default;

private:
    Array<RHIShaderResourceBinding> bindings_;
    VkDescriptorSetLayout handle_ = VK_NULL_HANDLE;
};

class VulkanShaderResourceGroup : public RHIShaderResourceGroup {
public:
    virtual void set_buffer(RHIBuffer* buffer, size_t binding, size_t offset, size_t range) override;

    inline uint32 get_index_pool() const {
        return index_pool_;
    }

    inline VkDescriptorSet& get_handle() {
        return descriptor_set_;
    }

    inline void initialize(size_t index_pool, VkDescriptorSet descriptor_set, RHIShaderResourceGroupLayout* layout) {
        descriptor_set_ = descriptor_set;
        layout_ = layout;
    }

    inline void reset() {
        initialize(0, VK_NULL_HANDLE, nullptr);
    }

    VulkanShaderResourceGroup() = default;
    virtual ~VulkanShaderResourceGroup() override = default;

private:
    RHIShaderResourceGroupLayout* layout_ = nullptr;
    VkDescriptorSet descriptor_set_ = VK_NULL_HANDLE;
    size_t index_pool_ = 0;
};

class VulkanShaderResourceGroupPool : public RHIShaderResourceGroupPool {
public:
    virtual RHIShaderResourceGroup* allocate_group(RHIShaderResourceGroupLayout* layout) override;
    virtual void deallocate_group(RHIShaderResourceGroup* group) override;
    virtual RHIShaderResourceGroup* get_group(size_t pool_index) override;

    void initialize(size_t max_sets, const Array<RHIShaderResourceBinding>& total_bindings);

    void destroy();

    VulkanShaderResourceGroupPool() = default;
    virtual ~VulkanShaderResourceGroupPool() override;

private:
    size_t next_index_to_allocate_ = 0; 
    HashSet<size_t> free_groups_; 
    Array<VulkanShaderResourceGroup> allocated_groups_;
    
    VkDescriptorSetLayout layout_ = VK_NULL_HANDLE;
    VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
    size_t max_sets_ = 0;
};

}  //namespace licht



