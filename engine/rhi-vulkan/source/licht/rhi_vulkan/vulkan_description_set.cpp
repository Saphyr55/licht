#include "licht/rhi_vulkan/vulkan_description_set.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_buffer.hpp"
#include "licht/core/memory/shared_ref_cast.hpp"


namespace licht {

void VulkanDescriptorSet::update(RHIBuffer* buffer, size_t binding, size_t offset, size_t range) {
    VulkanBuffer* vkbuffer = static_cast<VulkanBuffer*>(buffer);

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

RHIShaderResource* VulkanDescriptorPool::get_shader_resource(uint32 index) {
    return &descriptor_sets_[index];
}

void VulkanDescriptorPool::initialize() {

    uint32 count = descriptor_sets_.capacity();

    VkDescriptorPoolSize descriptor_pool_size = {};
    descriptor_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_pool_size.descriptorCount = count;

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
    descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_create_info.poolSizeCount = 1;
    descriptor_pool_create_info.pPoolSizes = &descriptor_pool_size;
    descriptor_pool_create_info.maxSets = count;

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateDescriptorPool(
        context_.device,
        &descriptor_pool_create_info,
        context_.allocator,
        &descriptor_pool_));

    Array<VkDescriptorSetLayout> layouts;
    layouts.resize(count, layout_);

    VkDescriptorSetAllocateInfo descriptor_set_allocation_info = {};
    descriptor_set_allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_set_allocation_info.descriptorPool = descriptor_pool_;
    descriptor_set_allocation_info.descriptorSetCount = count;
    descriptor_set_allocation_info.pSetLayouts = layouts.data();
    
    Array<VkDescriptorSet> vk_descriptor_sets;
    vk_descriptor_sets.resize(count);
    
    LICHT_VULKAN_CHECK(VulkanAPI::lvkAllocateDescriptorSets(
        context_.device,
        &descriptor_set_allocation_info,
        vk_descriptor_sets.data()));

    for (uint32 i = 0; i < count; i++) {
        descriptor_sets_.append(VulkanDescriptorSet(context_, vk_descriptor_sets[i]));
    }
}

void VulkanDescriptorPool::destroy() {
    VulkanAPI::lvkDestroyDescriptorPool(context_.device, descriptor_pool_, context_.allocator);
    descriptor_pool_ = VK_NULL_HANDLE;
}

VulkanDescriptorPool::VulkanDescriptorPool(VulkanContext& context, VkDescriptorSetLayout layout, size_t count)
    : context_(context)
    , layout_(layout)
    , descriptor_sets_(count)
    , descriptor_pool_(VK_NULL_HANDLE) {
}

VulkanDescriptorPool::~VulkanDescriptorPool() {
}

}  //namespace licht