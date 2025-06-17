#include "rhi_vulkan_debug_messenger.hpp"

#include <vulkan/vulkan_core.h>
#include "licht/core/defines.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_context.hpp"

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

void rhi_vulkan_debug_messenger_init(RHIVulkanContext* p_context) {
    LCHECK(p_context);

    LLOG_INFO("[rhi_vulkan_debug_messenger_init]", "Initializing Vulkan debug messenger...");

    PFN_vkCreateDebugUtilsMessengerEXT fvkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        p_context->rhi.licht_vkGetInstanceProcAddr(p_context->instance, "vkCreateDebugUtilsMessengerEXT"));
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

    fvkCreateDebugUtilsMessengerEXT(p_context->instance, &create_info, nullptr, &p_context->debug_utils_messenger);
    LCHECK(p_context->debug_utils_messenger != VK_NULL_HANDLE);

    LLOG_INFO("[rhi_vulkan_debug_messenger_init]", "Vulkan debug messenger initialized.");
}

void rhi_vulkan_debug_messenger_destroy(RHIVulkanContext* p_context) {
    LCHECK(p_context);
    
    LLOG_INFO("[rhi_vulkan_debug_messenger_destroy]", "Destroying Vulkan debug messenger...");

    // Destroy the debug messenger
    PFN_vkDestroyDebugUtilsMessengerEXT fvkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        p_context->rhi.licht_vkGetInstanceProcAddr(p_context->instance, "vkDestroyDebugUtilsMessengerEXT"));
    LCHECK(fvkDestroyDebugUtilsMessengerEXT);
    
    if (p_context->debug_utils_messenger != VK_NULL_HANDLE) {
        fvkDestroyDebugUtilsMessengerEXT(p_context->instance, p_context->debug_utils_messenger, nullptr);
        p_context->debug_utils_messenger = VK_NULL_HANDLE;
        LLOG_INFO("[rhi_vulkan_debug_messenger_destroy]", "Vulkan debug messenger destroyed.");
    } else {
        LLOG_WARN("[rhi_vulkan_debug_messenger_destroy]", "No Vulkan debug messenger to destroy.");
    }

}

}  //namespace licht