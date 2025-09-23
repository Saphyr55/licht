#include "licht/rhi_vulkan/rhi_vulkan_description_set.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_buffer.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"


namespace licht {

void RHIVulkanDescriptorSet::update(RHIBufferHandle buffer, size_t binding, size_t offset, size_t range) {
    RHIVulkanBufferRef vkbuffer = static_ref_cast<RHIVulkanBuffer>(buffer);

    VkDescriptorBufferInfo buffer_info = {};
    buffer_info.buffer = vkbuffer->get_handle();
    buffer_info.offset = offset;
    buffer_info.range = range;

    VkWriteDescriptorSet descriptor_write = {};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = descriptor_set_;
    descriptor_write.dstBinding = binding;
    descriptor_write.dstArrayElement = 0;

    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // TODO : Configurable
    descriptor_write.descriptorCount = 1;

    descriptor_write.pBufferInfo = &buffer_info;
    descriptor_write.pImageInfo = nullptr;        // TODO : Configurable
    descriptor_write.pTexelBufferView = nullptr;  // TODO : Configurable

    VulkanAPI::lvkUpdateDescriptorSets(context_.device, 1, &descriptor_write, 0, nullptr);
}

RHIDescriptorSetRef RHIVulkanDescriptorPool::get_descriptor_set(uint32 index) {
    return descriptor_sets_[index];
}

void RHIVulkanDescriptorPool::initialize() {

    uint32 image_count = information_.image_count;

    VkDescriptorPoolSize descriptor_pool_size = {};
    descriptor_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_pool_size.descriptorCount =image_count;

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
    descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_create_info.poolSizeCount = 1;
    descriptor_pool_create_info.pPoolSizes = &descriptor_pool_size;
    descriptor_pool_create_info.maxSets = image_count;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateDescriptorPool(context_.device, &descriptor_pool_create_info, context_.allocator, &descriptor_pool_))

    Array<VkDescriptorSetLayout> layouts;
    layouts.resize(image_count, layout_);

    VkDescriptorSetAllocateInfo descriptor_set_allocation_info = {};
    descriptor_set_allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_set_allocation_info.descriptorPool = descriptor_pool_;
    descriptor_set_allocation_info.descriptorSetCount = image_count;
    descriptor_set_allocation_info.pSetLayouts = layouts.data();
    
    Array<VkDescriptorSet> vk_descriptor_sets;
    vk_descriptor_sets.resize(image_count);
    
    LICHT_VULKAN_CHECK(VulkanAPI::lvkAllocateDescriptorSets(context_.device, &descriptor_set_allocation_info, vk_descriptor_sets.data()))

    descriptor_sets_.resize(image_count);
    for (uint32 i = 0; i < image_count; i++) {
        descriptor_sets_[i] = new_ref<RHIVulkanDescriptorSet>(context_, vk_descriptor_sets[i]);
    }

}

void RHIVulkanDescriptorPool::destroy() {
    if (descriptor_pool_ != VK_NULL_HANDLE) {
        VulkanAPI::lvkDestroyDescriptorPool(context_.device, descriptor_pool_, context_.allocator);
        descriptor_pool_ = VK_NULL_HANDLE;
    }
}

RHIVulkanDescriptorPool::RHIVulkanDescriptorPool(VulkanContext& context, VkDescriptorSetLayout layout, const RHIDescriptorSetInformation& information)
    : context_(context)
    , layout_(layout)
    , information_(information)
    , descriptor_sets_(information.image_count)
    , descriptor_pool_(VK_NULL_HANDLE) {
}

RHIVulkanDescriptorPool::~RHIVulkanDescriptorPool() {
}

}  //namespace licht