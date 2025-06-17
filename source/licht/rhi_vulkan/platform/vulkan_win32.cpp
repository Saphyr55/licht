#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef _WIN32

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>

#include "licht/core/trace/trace.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_context.hpp"

namespace licht {

void rhi_vulkan_surface_init(RHIVulkanContext* p_context, void* p_window_handle) {

    LCHECK(p_context);

    LLOG_INFO("[rhi_vulkan_surface_init]", "Initializing Vulkan surface...");

    PFN_vkCreateWin32SurfaceKHR create_win32_surface = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
        p_context->rhi.licht_vkGetInstanceProcAddr(p_context->instance, "vkCreateWin32SurfaceKHR"));

    // Construct the Win32 surface creation info
    VkWin32SurfaceCreateInfoKHR surface_create_info{};
    surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_create_info.pNext = nullptr;
    surface_create_info.hinstance = GetModuleHandle(nullptr);
    surface_create_info.hwnd = reinterpret_cast<HWND>(p_window_handle);
    surface_create_info.flags = 0;
    
    // Create the Vulkan surface
    LICHT_VULKAN_CHECK(create_win32_surface(p_context->instance, &surface_create_info,
                                           nullptr, &p_context->surface));
}

void rhi_vulkan_surface_destroy(RHIVulkanContext* p_context) {
    LCHECK(p_context);

    LLOG_INFO("[rhi_vulkan_surface_destroy]", "Destroying Vulkan surface...");

    if (p_context->surface != VK_NULL_HANDLE) {
        p_context->rhi.licht_vkDestroySurfaceKHR(p_context->instance, p_context->surface, nullptr);
        p_context->surface = VK_NULL_HANDLE;
        LLOG_INFO("[rhi_vulkan_surface_destroy]", "Vulkan surface destroyed.");
    }
    
}

}

#endif
