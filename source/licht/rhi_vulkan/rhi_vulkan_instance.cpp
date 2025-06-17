#include "rhi_vulkan_instance.hpp"
#include "licht/core/collection/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/string/format.hpp"
#include "licht/core/string/string.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

#if defined(_WIN32)
#include <windows.h>
#endif

#if defined(_WIN32)
#include <vulkan/vulkan_win32.h>
#endif

namespace licht {

void rhi_vulkan_instance_init(RHIVulkanContext* p_context) {
    LCHECK(p_context)

    uint32_t extension_count = 0;
    LICHT_VULKAN_CHECK(p_context->rhi.licht_vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));

    Array<VkExtensionProperties> available_extensions(extension_count);
    LICHT_VULKAN_CHECK(p_context->rhi.licht_vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.data()));

    Array<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    };

    for (const char* required_extension : extensions) {
        bool found = false;
        for (const VkExtensionProperties& available_extension : available_extensions) {
            LLOG_INFO("[rhi_vulkan_instance_init][vulkan_extension]", vformat("Available extension: %s", available_extension.extensionName));
            if (string_compare(required_extension, available_extension.extensionName) == 0) {
                found = true;
                break;
            }
        }
        LCHECK_MSG(found, "Required Vulkan extension not available");
    }

    Array<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

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
    create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    create_info.ppEnabledLayerNames = validation_layers.data();
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    LICHT_VULKAN_CHECK(p_context->rhi.licht_vkCreateInstance(&create_info, nullptr, &p_context->instance));
}

void rhi_vulkan_instance_destroy(RHIVulkanContext* p_context) {
    LCHECK(p_context)

    p_context->rhi.licht_vkDestroyInstance(p_context->instance, nullptr);
    p_context->instance = VK_NULL_HANDLE;
}

}  //namespace licht