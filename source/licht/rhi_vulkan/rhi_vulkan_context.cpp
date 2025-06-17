#include "rhi_vulkan_context.hpp"

#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/platform/dynamic_library.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_debug_messenger.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_instance.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_loader.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_surface.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

RHIVulkanContext* rhi_vulkan_context_create(void* p_window_handle) {
    LLOG_INFO("[rhi_vulkan_context_create]", "Initializing Vulkan RHI context...");

    RHIVulkanContext* context = Memory::new_resource<RHIVulkanContext>();
    
    bool library_loaded = rhi_vulkan_library_load(context);
    LLOG_FATAL_WHEN(!library_loaded, "[rhi_vulkan_context_create]", "Failed to load Vulkan RHI library.");

    bool core_functions_loaded = rhi_vulkan_core_load(context);
    LLOG_FATAL_WHEN(!core_functions_loaded, "[rhi_vulkan_context_create]", "Failed to load Vulkan RHI core functions.");

    context->instance = VK_NULL_HANDLE;
    context->device = VK_NULL_HANDLE;

    rhi_vulkan_instance_init(context);
    rhi_vulkan_instance_load(context);

    rhi_vulkan_debug_messenger_init(context);
    rhi_vulkan_surface_init(context, p_window_handle);

    return context;
}

void rhi_vulkan_context_destroy(RHIVulkanContext* context) {
    LCHECK(context)

    LLOG_INFO("[rhi_vulkan_context_destroy]", "Destroying Vulkan RHI context...");

    rhi_vulkan_surface_destroy(context);

    rhi_vulkan_debug_messenger_destroy(context);

    rhi_vulkan_instance_destroy(context);

    if (context->library) {
        DynamicLibraryLoader::unload(context->library);
        LLOG_INFO("[rhi_vulkan_context_destroy]", "Vulkan RHI context destroyed.");
    }

    Memory::delete_resource(context);
}

}  //namespace licht