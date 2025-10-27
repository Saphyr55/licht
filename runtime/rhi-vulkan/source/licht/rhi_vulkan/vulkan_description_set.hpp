#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/containers/hash_map.hpp"
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

    virtual const Array<RHIShaderResourceBinding>& get_bindings() const override {
        return bindings_;
    }

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
    virtual void set_buffer(const RHIWriteBufferResource& resource) override;

    virtual void set_sampler(const RHIWriteSamplerResource& resource) override;

    virtual void set_texture_sampler(const RHIWriteTextureSamplerResource& resource) override;

    virtual void set_texture(const RHIWriteTextureResource& resource) override;

    virtual void compile() override;

    inline uint32 get_index_pool() const {
        return index_pool_;
    }

    inline VkDescriptorSet& get_handle() {
        return descriptor_set_;
    }

    inline void initialize(size_t index_pool, RHIShaderResourceGroupLayout* layout) {
        layout_ = layout;
        index_pool_ = index_pool;
    }

    inline void reset() {
        initialize(0, nullptr);
        descriptor_set_ = VK_NULL_HANDLE;
    }

    VulkanShaderResourceGroup() = default;
    virtual ~VulkanShaderResourceGroup() override = default;

private:
    struct PendingDescriptorArray {
        VkDescriptorType type;
        Array<VkDescriptorBufferInfo> buffer_infos;
        Array<VkDescriptorImageInfo> image_infos;
    };

    HashMap<uint32, PendingDescriptorArray> pending_arrays_;

    RHIShaderResourceGroupLayout* layout_ = nullptr;
    VkDescriptorSet descriptor_set_ = VK_NULL_HANDLE;
    size_t index_pool_ = 0;
};

class VulkanShaderResourceGroupPool : public RHIShaderResourceGroupPool {
public:
    virtual RHIShaderResourceGroup* allocate_group(RHIShaderResourceGroupLayout* layout) override;
    virtual void deallocate_group(RHIShaderResourceGroup* group) override;

    virtual RHIShaderResourceGroup* get_group(size_t pool_index) override;

    virtual void dispose() override;

    virtual size_t get_count() override {
        return allocated_groups_.size();
    }

    virtual size_t get_max_count() override {
        return max_groups_;
    }

    void initialize(size_t max_sets, const Array<RHIShaderResourceBinding>& total_bindings);

    void destroy();

    VulkanShaderResourceGroupPool() = default;
    virtual ~VulkanShaderResourceGroupPool() override = default;

private:
    HashSet<size_t> free_groups_; 
    Array<VulkanShaderResourceGroup> allocated_groups_;
    VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
    size_t max_groups_ = 0;
    size_t next_index_to_allocate_ = 0;
};

}  //namespace licht



