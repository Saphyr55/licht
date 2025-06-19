#include "vulkan_context.hpp"

#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/platform/dynamic_library.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_debug_messenger.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_instance.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_loader.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_logical_device.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_physical_device.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_surface.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_swapchain.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

VulkanContext* vulkan_context_create(void* p_window_handle) {
    LLOG_INFO("[Vulkan]", "Initializing Vulkan RHI context...");

    VulkanContext* context = Memory::new_resource<VulkanContext>();
    
    bool library_loaded = vulkan_library_load(context);
    LLOG_FATAL_WHEN(!library_loaded, "[Vulkan]", "Failed to load Vulkan RHI library.");

    bool core_functions_loaded = vulkan_core_load(context);
    LLOG_FATAL_WHEN(!core_functions_loaded, "[Vulkan]", "Failed to load Vulkan RHI core functions.");

    context->instance = VK_NULL_HANDLE;
    context->device = VK_NULL_HANDLE;

    vulkan_instance_init(context);
    vulkan_instance_load(context);

    vulkan_debug_messenger_init(context);
    vulkan_surface_init(context, p_window_handle);

    vulkan_physical_device_init(context);
    vulkan_logical_device_init(context);

    vulkan_device_load(context);
    vulkan_queues_init(context);

    vulkan_swapchain_init(context);

    return context;
}

void vulkan_context_destroy(VulkanContext* p_context) {
    LCHECK(p_context)

    LLOG_INFO("[Vulkan]", "Destroying Vulkan RHI context...");

    vulkan_swapchain_destroy(p_context);
    vulkan_logical_device_destroy(p_context);
    vulkan_surface_destroy(p_context);
    vulkan_debug_messenger_destroy(p_context);
    vulkan_instance_destroy(p_context);

    if (p_context->library) {
        DynamicLibraryLoader::unload(p_context->library);
        LLOG_INFO("[Vulkan]", "Vulkan RHI context destroyed.");
    }

    Memory::delete_resource(p_context);
}

}  //namespace licht