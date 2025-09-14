#include <vulkan/vulkan_core.h>
#include "licht/core/defines.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

namespace licht {

VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT p_message_severity,
                                                        VkDebugUtilsMessageTypeFlagsEXT p_message_type,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
                                                        void* p_user_data) {
    switch (p_message_severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LLOG_INFO("[Vulkan]", p_callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LLOG_INFO("[Vulkan]", p_callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LLOG_WARN("[Vulkan]", p_callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LLOG_ERROR("[Vulkan]", p_callback_data->pMessage);
            break;
        default:
            LLOG_FATAL("[Vulkan]", "Unknown message severity.");
            break;
    }

    return VK_FALSE;
}

void vulkan_debug_messenger_init(VulkanContext& context) {
    LLOG_INFO("[Vulkan]", "Initializing Vulkan debug messenger...");

    PFN_vkCreateDebugUtilsMessengerEXT fvkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        VulkanAPI::lvkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT"));
    LCHECK(fvkCreateDebugUtilsMessengerEXT);

    VkDebugUtilsMessengerCreateInfoEXT create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = &debug_messenger_callback;
    create_info.pUserData = nullptr;

    fvkCreateDebugUtilsMessengerEXT(context.instance, &create_info, context.allocator, &context.debug_utils_messenger);
    LCHECK(context.debug_utils_messenger != VK_NULL_HANDLE);

    LLOG_INFO("[Vulkan]", "Vulkan debug messenger initialized.");
}

void vulkan_debug_messenger_destroy(VulkanContext& context) {

    LLOG_INFO("[Vulkan]", "Destroying Vulkan debug messenger...");

    // Destroy the debug messenger
    PFN_vkDestroyDebugUtilsMessengerEXT fvkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        VulkanAPI::lvkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT"));
    LCHECK(fvkDestroyDebugUtilsMessengerEXT);

    if (context.debug_utils_messenger != VK_NULL_HANDLE) {
        fvkDestroyDebugUtilsMessengerEXT(context.instance, context.debug_utils_messenger, context.allocator);
        context.debug_utils_messenger = VK_NULL_HANDLE;
        LLOG_INFO("[Vulkan]", "Vulkan debug messenger destroyed.");
    } else {
        LLOG_WARN("[Vulkan]", "No Vulkan debug messenger to destroy.");
    }
}

}  //namespace licht