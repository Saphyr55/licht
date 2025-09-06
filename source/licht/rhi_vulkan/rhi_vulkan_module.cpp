#include "rhi_vulkan_module.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/io/file_handle.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/platform/display.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/framebuffer.hpp"
#include "licht/rhi/pipeline/compiled_shader.hpp"
#include "licht/rhi/render_pass.hpp"
#include "licht/rhi/rhi.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_device.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_render_surface.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"
#include "licht/rhi_vulkan/vulkan_loader.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

void RHIVulkanModule::initialize() {
    LLOG_INFO("[RHIVulkanModule::initialize]", "Initializing Vulkan RHI module...");

    LLOG_FATAL_WHEN(!Display::get_default().is_valid(Display::MAIN_WINDOW_HANDLE), "[RHIVulkanModule::initialize]",
                    "Failed to retrieve a valid window handle. Ensure a window is created before initializing the Vulkan RHI module.");

    void* window_handle = Display::get_default().get_native_window_handle(Display::MAIN_WINDOW_HANDLE);

    vulkan_context_initialize(context_, window_handle);

    // -- Device --
    {
        device_ = new_ref<RHIVulkanDevice>(context_);
    }

    // -- Swapchain --
    {
        swapchain_ = device_->create_swapchain(surface_);
    }

    // -- Render Pass --
    {
        RHIAttachmentDescription render_pass_color_attachement = {};
        render_pass_color_attachement.format = swapchain_->get_format();

        RHIRenderPassDescription render_pass_description = {};
        render_pass_description.attachment_decriptions = {render_pass_color_attachement};

        render_pass_ = device_->create_render_pass(render_pass_description);
    }

    // -- Shaders reading --
    {
        FileSystem& file_system = FileSystem::get_platform();

        FileHandleResult vertex_file_open_error = file_system.open_read("shaders/main.vert.spv");
        LCHECK(vertex_file_open_error.has_value())
        SharedRef<FileHandle> vertex_file_handle = vertex_file_open_error.value();
        CompiledShader vertex_shader = CompiledShader(vertex_file_handle->read_all_bytes());

        FileHandleResult fragment_file_open_error = file_system.open_read("shaders/main.frag.spv");
        LCHECK(fragment_file_open_error.has_value())
        SharedRef<FileHandle> fragment_file_handle = fragment_file_open_error.value();
        CompiledShader fragment_shader = CompiledShader(fragment_file_handle->read_all_bytes());

        // -- Graphics Pipeline Shaders --
        RHIPipelineShaderStageCreateInfo vertex_stage_create_info;
        vertex_stage_create_info.name = "main";
        vertex_stage_create_info.shader = &vertex_shader;
        vertex_stage_create_info.type = ShaderStageType::Vertex;

        RHIPipelineShaderStageCreateInfo fragment_stage_create_info;
        fragment_stage_create_info.name = "main";
        fragment_stage_create_info.shader = &fragment_shader;
        fragment_stage_create_info.type = ShaderStageType::Fragment;

        // -- Graphics Pipeline --
        Viewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = swapchain_->get_width();
        viewport.height = swapchain_->get_height();
        viewport.min_depth = 0.0f;
        viewport.max_depth = 1.0f;

        Rect2D scissor = {};
        scissor.x = 0.0f;
        scissor.y = 0.0f;
        scissor.width = swapchain_->get_width();
        scissor.height = swapchain_->get_height();

        RHIPipelineViewportStateInformation viewport_info;
        viewport_info.viewport = viewport;
        viewport_info.scissor = scissor;

        RHIPipelineDescription pipeline_description;
        pipeline_description.render_pass = render_pass_;
        pipeline_description.vertex_shader_info = vertex_stage_create_info;
        pipeline_description.fragment_shader_info = fragment_stage_create_info;
        pipeline_description.viewport_info = viewport_info;
        pipeline_ = device_->create_graphics_pipeline(pipeline_description);
    }

    // -- Framebuffers --
    {
        for (RHITextureViewHandle texture : swapchain_->get_texture_views()) {
            RHIFramebufferDescription description = {};
            description.height = swapchain_->get_height();
            description.width = swapchain_->get_width();
            description.attachments = {texture};
            description.layers = 1;

            RHIFramebufferHandle framebuffer = device_->create_framebuffer(render_pass_, description);
        }
    }

    // -- Command Allocator
    {
        command_allocator_ = device_->create_command_allocator(frame_context_.frame_count);
    }

    // -- Frame Context Sync --
    {
        frame_context_.frame_available_semaphores.resize(frame_context_.frame_count);
        frame_context_.render_finished_semaphores.resize(frame_context_.frame_count);
        frame_context_.in_flight_fences.resize(frame_context_.frame_count);

        for (uint32 i = 0; i < frame_context_.frame_count; i++) {
            frame_context_.frame_available_semaphores[i] = device_->create_semaphore();
            frame_context_.render_finished_semaphores[i] = device_->create_semaphore();
            frame_context_.in_flight_fences[i] = device_->create_fence();
        }
    }
}

void RHIVulkanModule::tick() {
    RHICommandQueueRef graphics_queue = device_->query_queue(RHIQueueType::Graphics);
    RHICommandQueueRef present_queue = device_->query_queue(RHIQueueType::Transfer);

    swapchain_->acquire_next_frame(frame_context_);
    if (frame_context_.out_of_date || frame_context_.suboptimal || window_resized_) {
        device_->recreate_swapchain(swapchain_);
        window_resized_ = false;
        return;
    }

    // LICHT_VULKAN_CHECK(VulkanAPI::lvkResetFences(context_->device->get_handle(), 1, &context_->in_flight_fences[context_->current_frame]));

    RHICommandBufferHandle command_buffer = command_allocator_->open();
    command_allocator_->reset_command_buffer(command_buffer);

    Rect2D render_pass_area = {};
    render_pass_area.x = 0.0f;
    render_pass_area.y = 0.0f;
    render_pass_area.width = swapchain_->get_width();
    render_pass_area.height = swapchain_->get_height();

    RHIRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.render_pass = render_pass_;
    render_pass_begin_info.framebuffer = framebuffers_[frame_context_.current_frame];
    render_pass_begin_info.area = render_pass_area;

    command_buffer->begin();
    command_buffer->begin_render_pass(render_pass_begin_info);
    {
        command_buffer->bind_pipeline(pipeline_);

        Viewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = swapchain_->get_width();
        viewport.height = swapchain_->get_height();
        viewport.min_depth = 0.0f;
        viewport.max_depth = 1.0f;
        command_buffer->set_viewports(&viewport, 1);

        Rect2D scissor = {};
        scissor.x = 0.0f;
        scissor.y = 0.0f;
        scissor.width = swapchain_->get_width();
        scissor.height = swapchain_->get_height();
        command_buffer->set_scissors(&scissor, 1);

        RHIDrawCommand draw_command = {};
        draw_command.vertex_count = 3;
        draw_command.instance_count = 1;
        draw_command.first_instance = 0;
        draw_command.first_vertex = 0;
        command_buffer->draw(draw_command);
    }
    command_buffer->end_render_pass();
    command_buffer->end();

    RHISemaphoreHandle wait_semaphore = frame_context_.frame_available_semaphores[frame_context_.current_frame];
    RHISemaphoreHandle signal_semaphore = frame_context_.frame_available_semaphores[frame_context_.current_frame];
    RHIFenceHandle fence = frame_context_.in_flight_fences[frame_context_.current_frame];

    graphics_queue->submit({command_buffer}, {wait_semaphore}, {signal_semaphore}, fence);
    present_queue->present(swapchain_, {wait_semaphore}, frame_context_.current_frame);
    
    LICHT_VULKAN_CHECK(VulkanAPI::lvkWaitForFences(context_->device->get_handle(), 1, &context_->in_flight_fences[context_->current_frame], VK_TRUE, UINT64_MAX));
}

void RHIVulkanModule::shutdown() {
    device_->wait_idle();

    // -- Frame Context Sync --
    {
        for (uint32 i = 0; i < frame_context_.frame_count; i++) {
            device_->destroy_semaphore(frame_context_.frame_available_semaphores[i]);
            device_->destroy_semaphore(frame_context_.render_finished_semaphores[i]);
            device_->destroy_fence(frame_context_.in_flight_fences[i]);
        }
    }

    // -- Framebuffers --
    {
        for (RHIFramebufferHandle framebuffer : framebuffers_) {
            device_->destroy_framebuffer(framebuffer);
        }
    }

    device_->destroy_graphics_pipeline(pipeline_);
    device_->destroy_render_pass(render_pass_);
    device_->destroy_swapchain(swapchain_);

    vulkan_context_destroy(context_);

    LLOG_INFO("[RHIVulkanModule::shutdown]", "Shutting down Vulkan RHI module...");
}

}  //namespace licht