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
#include "licht/rhi_vulkan/vulkan_queue.hpp"
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

    context->library = vulkan_library_load();
    LLOG_FATAL_WHEN(!context->library, "[Vulkan]", "Failed to load Vulkan RHI library.");
    
    bool core_functions_loaded = vulkan_core_load(context->library);
    LLOG_FATAL_WHEN(!core_functions_loaded, "[Vulkan]", "Failed to load Vulkan RHI core functions.");

    context->instance = new_ref<VulkanInstance>();
    context->instance->initialize();
    context->instance->set_allocator(context->allocator);

    vulkan_instance_load(*context->instance);

    vulkan_debug_messenger_init(context);
    vulkan_surface_init(context, p_window_handle);

    context->device = new_ref<VulkanDevice>(*context->instance, context->surface, g_physical_device_extensions);
    context->device->initialize();
    vulkan_device_load(*context->device);

    VulkanQueue& gqueue = context->device->query_queue(VulkanQueueFamilyType::Graphics);
    VulkanQueue& pqueue = context->device->query_queue(VulkanQueueFamilyType::Present);
    
    vulkan_swapchain_init(context);

    vulkan_render_pass_init(context);

    FileHandleResult vertex_file_open_error = FileSystem::get_platform().open_read("shaders/main.vert.spv");
    LCHECK(vertex_file_open_error.has_value())
    SharedRef<FileHandle> vertex_file_handle = vertex_file_open_error.value();
    Array<uint8> vertex_code = vertex_file_handle->read_all_bytes();

    FileHandleResult fragment_file_open_error = FileSystem::get_platform().open_read("shaders/main.frag.spv");
    LCHECK(fragment_file_open_error.has_value())
    SharedRef<FileHandle> fragment_file_handle = fragment_file_open_error.value();
    Array<uint8> fragment_code = fragment_file_handle->read_all_bytes();

    VulkanShaderModule vertex_shader_module(vertex_code);
    VulkanShaderModule fragment_shader_module(fragment_code);

    context->graphics_pipeline = new_ref<VulkanGraphicsPipeline>("main", vertex_shader_module, fragment_shader_module);
    context->graphics_pipeline->initialize(context);

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
        LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateSemaphore(context->device->get_handle(), &semaphore_create_info, context->allocator, &context->image_available_semaphores[i]));
        LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateSemaphore(context->device->get_handle(), &semaphore_create_info, context->allocator, &context->render_finished_semaphores[i]));
        LICHT_VULKAN_CHECK(VulkanAPI::lvkCreateFence(context->device->get_handle(), &fence_create_info, context->allocator, &context->in_flight_fences[i]));
    }

    return context;
}

void vulkan_context_destroy(VulkanContext* p_context) {
    LCHECK(p_context)

    LLOG_INFO("[Vulkan]", "Destroying Vulkan RHI context...");
    for (usize i = 0; i < VulkanContext::MaxFrame; i++) {
        VulkanAPI::lvkDestroySemaphore(p_context->device->get_handle(), p_context->image_available_semaphores[i], p_context->allocator);
        VulkanAPI::lvkDestroySemaphore(p_context->device->get_handle(), p_context->render_finished_semaphores[i], p_context->allocator);
        VulkanAPI::lvkDestroyFence(p_context->device->get_handle(), p_context->in_flight_fences[i], p_context->allocator);
    }

    vulkan_command_pool_destroy(p_context);

    p_context->graphics_pipeline->destroy(p_context);

    vulkan_framebuffers_destroy(p_context);

    vulkan_render_pass_destroy(p_context);

    vulkan_swapchain_destroy(p_context);
    p_context->device->destroy();
    vulkan_surface_destroy(p_context);
    vulkan_debug_messenger_destroy(p_context);
    p_context->instance->destroy();

    if (p_context->library) {
        DynamicLibraryLoader::unload(p_context->library);
        LLOG_INFO("[Vulkan]", "Vulkan RHI context destroyed.");
    }

    Memory::delete_resource(p_context);
}

}  //namespace licht