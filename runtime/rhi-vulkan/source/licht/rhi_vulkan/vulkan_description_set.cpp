#include "licht/rhi_vulkan/vulkan_description_set.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi_vulkan/vulkan_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_sampler.hpp"
#include "licht/rhi_vulkan/vulkan_texture_view.hpp"

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

void VulkanShaderResourceGroup::set_buffer(const RHIWriteBufferResource& resource) {
    VulkanBuffer* vkbuffer = dynamic_cast<VulkanBuffer*>(resource.buffer);
    VkDescriptorType descriptor_type = vulkan_descriptor_type_get(layout_->get_resource_type(resource.binding));

    PendingDescriptorArray& entry = pending_arrays_[resource.binding];
    entry.type = descriptor_type;

    if (entry.buffer_infos.size() <= resource.array_index) {
        if (entry.buffer_infos.empty()) {
            entry.buffer_infos = {};
        }
        entry.buffer_infos.resize(resource.array_index + 1);
    }

    VkDescriptorBufferInfo& info = entry.buffer_infos[resource.array_index];
    info.buffer = vkbuffer->get_handle();
    info.offset = static_cast<VkDeviceSize>(resource.offset);
    info.range = static_cast<VkDeviceSize>(resource.range);
}

void VulkanShaderResourceGroup::set_sampler(const RHIWriteSamplerResource&  /*resource*/) {
    LCRASH("Not implemented.")
}

void VulkanShaderResourceGroup::set_texture_sampler(const RHIWriteTextureSamplerResource& resource) {
    VulkanTextureView* vk_view = dynamic_cast<VulkanTextureView*>(resource.texture_view);
    VulkanSampler* vk_sampler = dynamic_cast<VulkanSampler*>(resource.sampler);

    VkDescriptorType descriptor_type = vulkan_descriptor_type_get(layout_->get_resource_type(resource.binding));

    PendingDescriptorArray& entry = pending_arrays_[resource.binding];
    entry.type = descriptor_type;

    if (entry.image_infos.size() <= resource.array_index) {
        if (entry.image_infos.empty()) {
            entry.image_infos = {};
        }
        entry.image_infos.resize(resource.array_index + 1);
    }

    VkDescriptorImageInfo& info = entry.image_infos[resource.array_index];
    info.sampler = vk_sampler->get_handle();
    info.imageView = vk_view->get_handle();
    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void VulkanShaderResourceGroup::set_texture(const RHIWriteTextureResource&  /*resource*/) {
    LCRASH("Not implemented.")
}

void VulkanShaderResourceGroup::compile() {
    if (pending_arrays_.empty()) {
        return;
    }

    VulkanContext& context = vulkan_context_get();

    Array<VkWriteDescriptorSet> writes;
    writes.reserve(pending_arrays_.size());

    for (auto& [binding, entry] : pending_arrays_) {
        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = descriptor_set_;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = entry.type;

        if (!entry.buffer_infos.empty()) {
            write.descriptorCount = static_cast<uint32>(entry.buffer_infos.size());
            write.pBufferInfo = entry.buffer_infos.data();
        } else if (!entry.image_infos.empty()) {
            write.descriptorCount = static_cast<uint32>(entry.image_infos.size());
            write.pImageInfo = entry.image_infos.data();
        } else {
            continue;
        }

        writes.append(write);
    }

    VulkanAPI::lvkUpdateDescriptorSets(context.device,
                                       writes.size(),
                                       writes.data(),
                                       0,
                                       nullptr);

    pending_arrays_.clear();

}

RHIShaderResourceType VulkanShaderResourceGroupLayout::get_resource_type(size_t binding) const {
    for (const RHIShaderResourceBinding& shader_resource_binding : bindings_) {
        if (shader_resource_binding.binding == binding) {
            return shader_resource_binding.type;
        }
    }
    LCHECK_MSG(false, "Binding index not found in layout.");
    return RHIShaderResourceType::Uniform;
}

void VulkanShaderResourceGroupLayout::initialize(const Array<RHIShaderResourceBinding>& bindings) {
    VulkanContext& context = vulkan_context_get();
    bindings_ = bindings;

    Array<VkDescriptorSetLayoutBinding> layout_bindings = bindings_.map<VkDescriptorSetLayoutBinding>(binding_set_mapping);

    VkDescriptorSetLayoutCreateInfo descriptor_pool_layout_create_info = {};
    descriptor_pool_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_pool_layout_create_info.bindingCount = layout_bindings.size();
    descriptor_pool_layout_create_info.pBindings = layout_bindings.data();

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateDescriptorSetLayout(context.device,
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
    LCHECK(layout);

    VulkanContext& context = vulkan_context_get();

    VulkanShaderResourceGroupLayout* vk_layout = dynamic_cast<VulkanShaderResourceGroupLayout*>(layout);
    VkDescriptorSetLayout vk_layout_handle = vk_layout->get_handle();

    VkDescriptorSetAllocateInfo descriptor_set_allocation_info = {};
    descriptor_set_allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_set_allocation_info.descriptorPool = descriptor_pool_;
    descriptor_set_allocation_info.descriptorSetCount = 1;
    descriptor_set_allocation_info.pSetLayouts = &vk_layout_handle;

    size_t group_index;
    if (!free_groups_.empty()) {
        group_index = *free_groups_.begin();
        free_groups_.remove(group_index);
    } else {
        LCHECK(next_index_to_allocate_ < max_groups_);
        group_index = next_index_to_allocate_++;
    }

    VulkanShaderResourceGroup* new_group = &allocated_groups_[group_index];
    new_group->initialize(group_index, layout);

    LICHT_VULKAN_CHECK(VulkanAPI::lvkAllocateDescriptorSets(context.device,
                                                            &descriptor_set_allocation_info,
                                                            &new_group->get_handle()));

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
    LCHECK(group);

    VulkanContext& context = vulkan_context_get();
    VulkanShaderResourceGroup* vk_group = dynamic_cast<VulkanShaderResourceGroup*>(group);
    VkDescriptorSet& descriptor_set = vk_group->get_handle();

    free_groups_.add(vk_group->get_index_pool());

    LICHT_VULKAN_CHECK(VulkanAPI::lvkFreeDescriptorSets(context.device,
                                                        descriptor_pool_,
                                                        1,
                                                        &descriptor_set));

    vk_group->reset();
}

void VulkanShaderResourceGroupPool::dispose() {
    for (size_t i = 0; i < get_count(); i++) {
        deallocate_group(get_group(i));
    }
    free_groups_.clear();
    allocated_groups_.clear();
}

void VulkanShaderResourceGroupPool::initialize(size_t max_groups, const Array<RHIShaderResourceBinding>& total_bindings) {
    max_groups_ = max_groups;

    // free_groups_ doesn't need to be resize; its size is determined by the number of elements added.
    // next_index_to_allocate_ is implicitly 0 at start.
    next_index_to_allocate_ = 0;
    allocated_groups_.resize(max_groups);

    VulkanContext& context = vulkan_context_get();

    Array<VkDescriptorPoolSize> pool_sizes;

    HashMap<VkDescriptorType, uint32> type_counts;
    for (const RHIShaderResourceBinding& binding : total_bindings) {
        type_counts[vulkan_descriptor_type_get(binding.type)] += binding.count;
    }

    for (const auto& [type, count] : type_counts) {
        VkDescriptorPoolSize pool_size = {};
        pool_size.type = type;
        pool_size.descriptorCount = count * static_cast<uint32>(max_groups);
        pool_sizes.append(pool_size);
    }

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
    descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptor_pool_create_info.poolSizeCount = pool_sizes.size();
    descriptor_pool_create_info.pPoolSizes = pool_sizes.data();
    descriptor_pool_create_info.maxSets = static_cast<uint32>(max_groups);

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateDescriptorPool(context.device,
                                                          &descriptor_pool_create_info,
                                                          context.allocator,
                                                          &descriptor_pool_));
}

void VulkanShaderResourceGroupPool::destroy() {
    if (descriptor_pool_ != VK_NULL_HANDLE) {
        VulkanContext& context = vulkan_context_get();
        VulkanAPI::lvkDestroyDescriptorPool(context.device, descriptor_pool_, context.allocator);
        descriptor_pool_ = VK_NULL_HANDLE;
        next_index_to_allocate_ = 0;
    }
}

}  //namespace licht