#include "vulkan_rhi.hpp"

#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

namespace licht {

Array<VkExtensionProperties> VulkanRHI::get_instance_extension_properties() {

    uint32 extension_count = 0;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));

    Array<VkExtensionProperties> available_extensions;
    available_extensions.resize(extension_count);
    LICHT_VULKAN_CHECK(VulkanAPI::lvkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.data()));

    return available_extensions;
}

}