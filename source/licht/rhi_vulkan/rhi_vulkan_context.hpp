#pragma once

#include "licht/core/trace/trace.hpp"
#include "licht/platform/dynamic_library.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_functions.hpp"
#include "vulkan/vulkan_core.h"

#define LICHT_VULKAN_CHECK(Expr) LCHECK((Expr) == VK_SUCCESS)

namespace licht {

struct RHIVulkanContext {
    SharedRef<DynamicLibrary> library;

    RHIVulkanFuntions rhi;

    VkInstance instance = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkDebugUtilsMessengerEXT debug_utils_messenger = VK_NULL_HANDLE;
};

RHIVulkanContext* rhi_vulkan_context_create(void* p_window_handle);

void rhi_vulkan_context_destroy(RHIVulkanContext* context);

}