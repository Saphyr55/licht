#include "licht/rhi_vulkan/vulkan_shader_module.hpp"
#include "licht/core/defines.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

VulkanShaderModule::VulkanShaderModule(const Array<uint8>& p_code)
    : code_(p_code)
    , handle_(VK_NULL_HANDLE) {
}

void VulkanShaderModule::initialize(VulkanContext& context) {

    if (handle_ != VK_NULL_HANDLE) {
        return;
    }

    VkShaderModuleCreateInfo shader_module_create_info = {};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = code_.size();
    shader_module_create_info.pCode = reinterpret_cast<const uint32*>(code_.data());

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateShaderModule(context.device, &shader_module_create_info, context.allocator, &handle_));
}

void VulkanShaderModule::destroy(VulkanContext& context) {
    VulkanAPI::lvkDestroyShaderModule(context.device, handle_, context.allocator);
}

VkShaderModule VulkanShaderModule::handle() {
    return handle_;
}

}  //namespace licht