#include "licht/rhi_vulkan/vulkan_description_set.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/vulkan_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "vulkan/vulkan_core.h"

namespace licht {

constexpr static auto binding_set_mapping = [](const RHIShaderResourceBinding& binding) {
    VkDescriptorType descriptor_type = vulkan_descriptor_type_get(binding.type);
    VkShaderStageFlags stage_flags = vulkan_shader_stage_get(binding.stage);

    VkDescriptorSetLayoutBinding layout_binding = {};
    layout_binding.binding = binding.binding;
    layout_binding.descriptorType = descriptor_type;
    layout_binding.descriptorCount = binding.count;
    layout_binding.stageFlags = stage_flags;
    layout_binding.pImmutableSamplers = nullptr;

    return layout_binding;
};

RHIShaderResourceType VulkanShaderResourceGroupLayout::get_resource_type(size_t binding) const {
    for (const RHIShaderResourceBinding& shader_resource_binding : bindings_) {
        if (shader_resource_binding.binding == binding) {
            return shader_resource_binding.type;
        }
    }
    LCHECK_MSG(false, "Binding index not found in layout.");
    return RHIShaderResourceType::Uniform;
}

void VulkanShaderResourceGroup::set_buffer(RHIBuffer* buffer, size_t binding, size_t offset, size_t range) {
    VulkanBuffer* vkbuffer = static_cast<VulkanBuffer*>(buffer);
    VulkanContext& context = vulkan_context_get();

    VkDescriptorBufferInfo buffer_info = {};
    buffer_info.buffer = vkbuffer->get_handle();
    buffer_info.offset = offset;
    buffer_info.range = range;

    VkWriteDescriptorSet descriptor_write = {};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = descriptor_set_;
    descriptor_write.dstBinding = binding;
    descriptor_write.dstArrayElement = 0;

    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;  // TODO : Configurable
    descriptor_write.descriptorCount = 1;

    descriptor_write.pBufferInfo = &buffer_info;
    descriptor_write.pImageInfo = nullptr;        // TODO : Configurable
    descriptor_write.pTexelBufferView = nullptr;  // TODO : Configurable

    VulkanAPI::lvkUpdateDescriptorSets(context.device, 1, &descriptor_write, 0, nullptr);
}

void VulkanShaderResourceGroupLayout::initialize(const Array<RHIShaderResourceBinding>& bindings) {
    bindings_ = bindings;
    VulkanContext& context = vulkan_context_get();

    Array<VkDescriptorSetLayoutBinding> layout_bindings = bindings_.map<VkDescriptorSetLayoutBinding>(binding_set_mapping);

    VkDescriptorSetLayoutCreateInfo descriptor_pool_layout_create_info = {};
    descriptor_pool_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_pool_layout_create_info.bindingCount = layout_bindings.size();
    descriptor_pool_layout_create_info.pBindings = layout_bindings.data();

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateDescriptorSetLayout(
        context.device,
        &descriptor_pool_layout_create_info,
        context.allocator,
        &handle_));
}

void VulkanShaderResourceGroupLayout::destroy() {
    if (handle_ != VK_NULL_HANDLE) {
        VulkanContext& context = vulkan_context_get();
        VulkanAPI::lvkDestroyDescriptorSetLayout(context.device, handle_, context.allocator);
        handle_ = VK_NULL_HANDLE;
    }
}

RHIShaderResourceGroup* VulkanShaderResourceGroupPool::allocate_group(RHIShaderResourceGroupLayout* layout) {
    LCHECK(layout)

    VulkanContext& context = vulkan_context_get();

    VulkanShaderResourceGroupLayout* vk_layout = static_cast<VulkanShaderResourceGroupLayout*>(layout);
    VkDescriptorSetLayout vk_layout_handle = vk_layout->get_handle();

    VkDescriptorSetAllocateInfo descriptor_set_allocation_info = {};
    descriptor_set_allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_set_allocation_info.descriptorPool = descriptor_pool_;
    descriptor_set_allocation_info.descriptorSetCount = 1;
    descriptor_set_allocation_info.pSetLayouts = &vk_layout_handle;

    VkDescriptorSet vk_descriptor_set = VK_NULL_HANDLE;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkAllocateDescriptorSets(
        context.device,
        &descriptor_set_allocation_info,
        &vk_descriptor_set));

    size_t group_index;
    if (!free_groups_.empty()) {
        group_index = *free_groups_.begin();
        free_groups_.remove(group_index);
    } else {
        LCHECK(next_index_to_allocate_ < max_sets_);
        group_index = next_index_to_allocate_++;
    }

    VulkanShaderResourceGroup* new_group = &allocated_groups_[group_index];
    new_group->initialize(group_index, vk_descriptor_set, layout);

    return new_group;
}

RHIShaderResourceGroup* VulkanShaderResourceGroupPool::get_group(size_t pool_index) {
    VulkanShaderResourceGroup& group = allocated_groups_[pool_index]; 
    if (group.get_handle() == VK_NULL_HANDLE) {
        return nullptr;
    }
    return &group;
}

void VulkanShaderResourceGroupPool::deallocate_group(RHIShaderResourceGroup* group) {
    LCHECK(group)

    VulkanContext& context = vulkan_context_get();
    VulkanShaderResourceGroup* vk_group = static_cast<VulkanShaderResourceGroup*>(group);
    VkDescriptorSet& descriptor_set = vk_group->get_handle();

    free_groups_.add(vk_group->get_index_pool());

    LICHT_VULKAN_CHECK(VulkanAPI::lvkFreeDescriptorSets(
        context.device,
        descriptor_pool_,
        1,
        &descriptor_set));

    vk_group->reset();
}

void VulkanShaderResourceGroupPool::initialize(size_t max_sets, const Array<RHIShaderResourceBinding>& total_bindings) {
    max_sets_ = max_sets;

    // free_groups_ doesn't need resize; its size is determined by the number of elements added.
    // next_index_to_allocate_ is implicitly 0 at start.
    next_index_to_allocate_ = 0;
    allocated_groups_.resize(max_sets);

    VulkanContext& context = vulkan_context_get();

    Array<VkDescriptorPoolSize> pool_sizes;

    HashMap<VkDescriptorType, uint32> type_counts;
    for (const auto& binding : total_bindings) {
        type_counts[vulkan_descriptor_type_get(binding.type)] += binding.count;
    }

    for (const auto& [type, count] : type_counts) {
        VkDescriptorPoolSize pool_size = {};
        pool_size.type = type;
        pool_size.descriptorCount = count * static_cast<uint32>(max_sets);
        pool_sizes.append(pool_size);
    }

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
    descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_create_info.poolSizeCount = pool_sizes.size();
    descriptor_pool_create_info.pPoolSizes = pool_sizes.data();
    descriptor_pool_create_info.maxSets = static_cast<uint32>(max_sets);
    descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateDescriptorPool(
        context.device,
        &descriptor_pool_create_info,
        context.allocator,
        &descriptor_pool_));
}

void VulkanShaderResourceGroupPool::destroy() {
    if (descriptor_pool_ != VK_NULL_HANDLE) {
        VulkanContext& context = vulkan_context_get();
        VulkanAPI::lvkDestroyDescriptorPool(context.device, descriptor_pool_, context.allocator);
        descriptor_pool_ = VK_NULL_HANDLE;
        allocated_groups_.clear();
        free_groups_.clear();
        next_index_to_allocate_ = 0;
    }
}

VulkanShaderResourceGroupPool::~VulkanShaderResourceGroupPool() {
}

}  //namespace licht