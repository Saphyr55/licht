#include "vulkan_context.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/platform/dynamic_library.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_command_pool.hpp"
#include "licht/rhi_vulkan/vulkan_debug_messenger.hpp"
#include "licht/rhi_vulkan/vulkan_framebuffer.hpp"
#include "licht/rhi_vulkan/vulkan_graphics_pipeline.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"
#include "licht/rhi_vulkan/vulkan_queue.hpp"
#include "licht/rhi_vulkan/vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_shader_module.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

void vulkan_context_initialize(VulkanContext& context, void* window_handle) {
    LLOG_INFO("[Vulkan]", "Initializing Vulkan RHI context...");

    {
        context.library = vulkan_library_load();
        LLOG_FATAL_WHEN(!context.library, "[Vulkan]", "Failed to load Vulkan RHI library.");

        bool core_functions_loaded = vulkan_core_api_load(context.library);
        LLOG_FATAL_WHEN(!core_functions_loaded, "[Vulkan]", "Failed to load Vulkan RHI core functions.");

        vulkan_instance_initialize(context);

        vulkan_debug_messenger_init(context);

        context.surface = RHIVulkanRenderSurface::create(context.instance, window_handle);

        VulkanPhysicalDeviceSelector selector(context, g_physical_device_extensions);
        vulkan_device_initialize(context, selector);
    }
    {
        /*
        vulkan_render_pass_init(context);

        context.graphics_pipeline = VulkanGraphicsPipeline::create("main", "shaders/main.vert.spv", "shaders/main.frag.spv");
        context.graphics_pipeline->initialize(context);

        vulkan_framebuffers_init(context);

        vulkan_command_pool_init(context);
        vulkan_command_buffer_init(context);

        VkSemaphoreCreateInfo semaphore_create_info = {};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_create_info{};
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        context.image_available_semaphores.resize(VulkanContext::MaxFrame);
        context.render_finished_semaphores.resize(VulkanContext::MaxFrame);
        context.in_flight_fences.resize(VulkanContext::MaxFrame);

        for (usize i = 0; i < VulkanContext::MaxFrame; i++) {
            LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateSemaphore(context.device->get_handle(), &semaphore_create_info, context.allocator, &context.image_available_semaphores[i]));
            LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateSemaphore(context.device->get_handle(), &semaphore_create_info, context.allocator, &context.render_finished_semaphores[i]));
            LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateFence(context.device->get_handle(), &fence_create_info, context.allocator, &context.in_flight_fences[i]));
        }
         */
    }
}

void vulkan_context_destroy(VulkanContext& context) {
    /*
    LLOG_INFO("[Vulkan]", "Destroying Vulkan RHI context...");
    for (usize i = 0; i < VulkanContext::MaxFrame; i++) {
        VulkanAPI::lvkDestroySemaphore(p_context.device->get_handle(), p_context.image_available_semaphores[i], p_context.allocator);
        VulkanAPI::lvkDestroySemaphore(p_context.device->get_handle(), p_context.render_finished_semaphores[i], p_context.allocator);
        VulkanAPI::lvkDestroyFence(p_context.device->get_handle(), p_context.in_flight_fences[i], p_context.allocator);
    }
    vulkan_command_pool_destroy(p_context);

    p_context.graphics_pipeline->destroy(p_context);

    vulkan_framebuffers_destroy(p_context);

    vulkan_render_pass_destroy(p_context);

    p_context.device->destroy_swapchain(p_context.swapchain);
*/

    vulkan_device_destroy(context);

    context.surface->destroy();

    vulkan_debug_messenger_destroy(context);

    vulkan_instance_destroy(context);

    if (context.library) {
        DynamicLibraryLoader::unload(context.library);
        LLOG_INFO("[Vulkan]", "Vulkan RHI context destroyed.");
    }
}

const char* vulkan_string_of_present_mode(VkPresentModeKHR present_mode) {
    switch (present_mode) {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            return "VK_PRESENT_MODE_IMMEDIATE_KHR";
        case VK_PRESENT_MODE_MAILBOX_KHR:
            return "VK_PRESENT_MODE_MAILBOX_KHR";
        case VK_PRESENT_MODE_FIFO_KHR:
            return "VK_PRESENT_MODE_FIFO_KHR";
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            return "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
        case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
            return "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR";
        case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
            return "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR";
        default:
            return "VK_UNKNOWN_PRESENT_MODE";
    }
}

const char* vulkan_string_of_result(VkResult result) {
    switch (result) {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        case VK_NOT_READY:
            return "VK_NOT_READY";
        case VK_TIMEOUT:
            return "VK_TIMEOUT";
        case VK_EVENT_SET:
            return "VK_EVENT_SET";
        case VK_EVENT_RESET:
            return "VK_EVENT_RESET";
        case VK_INCOMPLETE:
            return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:
            return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:
            return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
            return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        default:
            return "VK_UNKNOWN_ERROR";
    }
}
}  //namespace licht