#include "licht/rhi_vulkan/rhi_vulkan_description_set.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

void RHIVulkanDescriptorSetLayout::initialize() {

    static constexpr auto desc_set_mapping = [&](const RHIDescriptorSetLayoutBinding& binding) {
        VkDescriptorSetLayoutBinding layout_binding = {};
        layout_binding.binding = binding.binding;
        layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layout_binding.descriptorCount = binding.count;
        layout_binding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
        layout_binding.pImmutableSamplers = nullptr;  // TODO: Supports texture.
        return layout_binding;
    };

    Array<VkDescriptorSetLayoutBinding> layout_bindings = bindings_.map<VkDescriptorSetLayoutBinding>(desc_set_mapping);

    VkDescriptorSetLayoutCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create_info.bindingCount = layout_bindings.size();
    create_info.pBindings = layout_bindings.data();

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateDescriptorSetLayout(context_.device, &create_info, context_.allocator, &handle_))
}

void RHIVulkanDescriptorSetLayout::destroy() {
    if (handle_ != VK_NULL_HANDLE) {
        VulkanAPI::lvkDestroyDescriptorSetLayout(context_.device, handle_, context_.allocator);
        handle_ = VK_NULL_HANDLE;
    }
}

RHIVulkanDescriptorSetLayout::RHIVulkanDescriptorSetLayout(VulkanContext& context, const Array<RHIDescriptorSetLayoutBinding>& bindings)
    : context_(context)
    , bindings_(bindings)
    , handle_(VK_NULL_HANDLE) {
}

RHIVulkanDescriptorSetLayout::~RHIVulkanDescriptorSetLayout() {
    destroy();
}

}