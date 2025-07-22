#include "vulkan_instance.hpp"

#include "licht/core/collection/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/string/format.hpp"

#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

#if defined(_WIN32)
#include <windows.h>
#endif

#if defined(_WIN32)
#include <vulkan/vulkan_win32.h>
#endif

namespace licht {

void VulkanInstance::initialize(const VkApplicationInfo& p_info, const VkInstanceCreateInfo& p_create_info) {
    uint32 extension_count = 0;
    LICHT_VULKAN_CHECK(VulkanAPI::lvkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));

    Array<VkExtensionProperties> available_extensions(extension_count);
    LICHT_VULKAN_CHECK(VulkanAPI::lvkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.data()));

    Array<const char*> desired_extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    };

    for (const char* required_extension : desired_extensions) {
        LCHECK_MSG(desired_extensions.contains(required_extension, [](const char* a, const char* b) -> int32 {
            return string_compare(a, b);
        }),
                   vformat("Required Vulkan extension '%s' is not available.", required_extension));
    }

    Array<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;
    app_info.pApplicationName = "licht";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "licht";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledLayerCount = static_cast<uint32>(validation_layers.size());
    create_info.ppEnabledLayerNames = validation_layers.data();
    create_info.enabledExtensionCount = static_cast<uint32>(desired_extensions.size());
    create_info.ppEnabledExtensionNames = desired_extensions.data();

    LLOG_INFO("[Vulkan]", "Creating Vulkan instance with the following extensions:");
    for (const char* extension : desired_extensions) {
        LLOG_INFO("[Vulkan]", vformat("  - %s", extension));
    }

    LLOG_INFO("[Vulkan]", "Using the following validation layers:");
    for (const char* layer : validation_layers) {
        LLOG_INFO("[Vulkan]", vformat("  - %s", layer));
    }

    LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateInstance(&create_info, allocator_, &handle_));

    LLOG_INFO("[Vulkan]", "Vulkan instance created successfully.");
}

void VulkanInstance::destroy() {
    LLOG_INFO("[Vulkan]", "Destroying Vulkan instance...");
    VulkanAPI::lvkDestroyInstance(handle_, allocator_);
    handle_ = VK_NULL_HANDLE;
    LLOG_INFO("[Vulkan]", "Vulkan instance destroyed.");
}

}  //namespace licht