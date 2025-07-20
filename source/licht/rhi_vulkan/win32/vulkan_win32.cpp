#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef _WIN32

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>

#include "licht/core/trace/trace.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

namespace licht {

void vulkan_surface_init(VulkanContext* p_context, void* p_window_handle) {

    LCHECK(p_context);

    LLOG_INFO("[Vulkan]", "Initializing Vulkan surface...");

    PFN_vkCreateWin32SurfaceKHR licht_vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
        VulkanAPI::lvkGetInstanceProcAddr(p_context->instance, "vkCreateWin32SurfaceKHR"));

    VkWin32SurfaceCreateInfoKHR surface_create_info = {};
    surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_create_info.pNext = nullptr;
    surface_create_info.hinstance = ::GetModuleHandle(nullptr);
    surface_create_info.hwnd = (HWND) p_window_handle;
    surface_create_info.flags = 0;
    
    LICHT_VULKAN_CHECK(licht_vkCreateWin32SurfaceKHR(p_context->instance, &surface_create_info, p_context->allocator, &p_context->surface));
}

void vulkan_surface_destroy(VulkanContext* p_context) {
    LCHECK(p_context);

    LLOG_INFO("[Vulkan]", "Destroying Vulkan surface...");

    if (p_context->surface != VK_NULL_HANDLE) {
        VulkanAPI::lvkDestroySurfaceKHR(p_context->instance, p_context->surface, p_context->allocator);
        p_context->surface = VK_NULL_HANDLE;
        LLOG_INFO("[Vulkan]", "Vulkan surface destroyed.");
    }
    
}

}

#endif
