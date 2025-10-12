#include "licht/rhi_vulkan/win32/vulkan_win32.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi_vulkan/vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef _WIN32

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>

#include "licht/core/trace/trace.hpp"

namespace licht {

SharedRef<VulkanRenderSurface> VulkanRenderSurface::create(VulkanContext& context, void* window_handle) {
    return new_ref<RHIVulkanWindowsSurface>(context, window_handle);
}

RHIVulkanWindowsSurface::RHIVulkanWindowsSurface(VulkanContext& context, void* p_window_handle)
 : VulkanRenderSurface(context.instance, context.allocator, p_window_handle) {
}

void RHIVulkanWindowsSurface::initialize() {
    LLOG_INFO("[Vulkan]", "Initializing Vulkan surface...");

    PFN_vkCreateWin32SurfaceKHR licht_vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
        VulkanAPI::lvkGetInstanceProcAddr(instance_, "vkCreateWin32SurfaceKHR"));

    VkWin32SurfaceCreateInfoKHR surface_create_info = {};
    surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_create_info.pNext = nullptr;
    surface_create_info.hinstance = ::GetModuleHandle(nullptr);
    surface_create_info.hwnd = (HWND) window_handle_;
    surface_create_info.flags = 0;

    LICHT_VULKAN_CHECK(licht_vkCreateWin32SurfaceKHR(instance_, &surface_create_info, allocator_, &handle_));
}

void RHIVulkanWindowsSurface::destroy() {
    LLOG_INFO("[Vulkan]", "Destroying Vulkan surface...");

    if (handle_ != VK_NULL_HANDLE) {
        VulkanAPI::lvkDestroySurfaceKHR(instance_, handle_, allocator_);
        handle_ = VK_NULL_HANDLE;
        LLOG_INFO("[Vulkan]", "Vulkan surface destroyed.");
    }
}

}

#endif
