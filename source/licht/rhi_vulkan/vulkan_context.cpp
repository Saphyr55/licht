#include "vulkan_context.hpp"

#include "licht/core/collection/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/io/file_handle.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/platform/dynamic_library.hpp"
#include "licht/rhi_vulkan/vulkan_command_buffer.hpp"
#include "licht/rhi_vulkan/vulkan_command_pool.hpp"
#include "licht/rhi_vulkan/vulkan_debug_messenger.hpp"
#include "licht/rhi_vulkan/vulkan_framebuffer.hpp"
#include "licht/rhi_vulkan/vulkan_graphics_pipeline.hpp"
#include "licht/rhi_vulkan/vulkan_instance.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"
#include "licht/rhi_vulkan/vulkan_logical_device.hpp"
#include "licht/rhi_vulkan/vulkan_physical_device.hpp"
#include "licht/rhi_vulkan/vulkan_render_pass.hpp"
#include "licht/rhi_vulkan/vulkan_shader_module.hpp"
#include "licht/rhi_vulkan/vulkan_surface.hpp"
#include "licht/rhi_vulkan/vulkan_swapchain.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

uint32 VulkanContext::MaxFrame = 2;

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

    vulkan_render_pass_init(context);

    FileOpenError<SharedRef<FileHandle>> vertex_file_open_error = FileSystem::get_platform().open_read("shaders/main.vert.spv");
    LCHECK(vertex_file_open_error.has_value())
    SharedRef<FileHandle> vertex_file_handle = vertex_file_open_error.value();
    Array<uint8> vertex_code = vertex_file_handle->read_all_bytes();

    FileOpenError<SharedRef<FileHandle>> fragment_file_open_error = FileSystem::get_platform().open_read("shaders/main.frag.spv");
    LCHECK(fragment_file_open_error.has_value())
    SharedRef<FileHandle> fragment_file_handle = fragment_file_open_error.value();
    Array<uint8> fragment_code = fragment_file_handle->read_all_bytes();

    VulkanShaderModule vertex_shader_module(vertex_code);
    VulkanShaderModule fragment_shader_module(fragment_code);

    context->graphics_pipeline = new_ref<VulkanGraphicsPipeline>("main", vertex_shader_module, fragment_shader_module);
    context->graphics_pipeline->init(context);

    vulkan_framebuffers_init(context);

    vulkan_command_pool_init(context);
    vulkan_command_buffer_init(context);

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_create_info{};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    context->image_available_semaphores.resize(VulkanContext::MaxFrame);
    context->render_finished_semaphores.resize(VulkanContext::MaxFrame);
    context->in_flight_fences.resize(VulkanContext::MaxFrame);

    for (usize i = 0; i < VulkanContext::MaxFrame; i++) {
        LICHT_VULKAN_CHECK(context->api.licht_vkCreateSemaphore(context->device, &semaphore_create_info, context->allocator, &context->image_available_semaphores[i]));
        LICHT_VULKAN_CHECK(context->api.licht_vkCreateSemaphore(context->device, &semaphore_create_info, context->allocator, &context->render_finished_semaphores[i]));
        LICHT_VULKAN_CHECK(context->api.licht_vkCreateFence(context->device, &fence_create_info, context->allocator, &context->in_flight_fences[i]));
    }

    return context;
}

void vulkan_context_destroy(VulkanContext* p_context) {
    LCHECK(p_context)

    LLOG_INFO("[Vulkan]", "Destroying Vulkan RHI context...");
    for (usize i = 0; i < VulkanContext::MaxFrame; i++) {
        p_context->api.licht_vkDestroySemaphore(p_context->device, p_context->image_available_semaphores[i], p_context->allocator);
        p_context->api.licht_vkDestroySemaphore(p_context->device, p_context->render_finished_semaphores[i], p_context->allocator);
        p_context->api.licht_vkDestroyFence(p_context->device, p_context->in_flight_fences[i], p_context->allocator);
    }

    vulkan_command_pool_destroy(p_context);

    p_context->graphics_pipeline->destroy(p_context);

    vulkan_framebuffers_destroy(p_context);

    vulkan_render_pass_destroy(p_context);

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