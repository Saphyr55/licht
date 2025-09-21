#include "render_frame_script.hpp"
#include "ludo_types.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/io/file_handle.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/display.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/math/common_math.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/rhi_module.hpp"
#include "licht/rhi/rhi_types.hpp"

#include <chrono>
#include <cstddef>

namespace licht {

RenderFrameScript::RenderFrameScript()
    : window_handle_(Display::InvalidWindowHandle)
    , device_(nullptr)
    , swapchain_(nullptr)
    , render_pass_(nullptr)
    , pipeline_(nullptr)
    , framebuffer_memory_allocator_(1024 /* 1 kB */)
    , framebuffers_(4, RHIFramebufferAllocator(framebuffer_memory_allocator_))
    , graphics_command_allocator_(nullptr)
    , frame_context_()
    , pause_(false)
    , window_resized_(false) {
}

RenderFrameScript::~RenderFrameScript() {}

void RenderFrameScript::on_startup() {
    ModuleRegistry& registry = ModuleRegistry::get_instance();
    RHIModule* module = registry.get_module<RHIModule>(RHIModule::ModuleName);

    // -- Device --
    {
        device_ = module->get_device();
        LCHECK(device_);
    }

    // -- Window Handle --
    {
        window_handle_ = module->get_window_handle();
        LLOG_FATAL_WHEN(!Display::get_default().is_valid(window_handle_), "[RHIModule]",
                        "Failed to retrieve a valid window handle. Ensure a window is created before initializing the RHI Module.");
    }

    // -- Vertices data --
    {
        positions_ = {
            Vector3f(-0.5f, -0.5f, 0.0f),// Top Left
            Vector3f(0.5f, -0.5f, 0.0f), // Top Right
            Vector3f(0.5f, 0.5f, 0.0f),  // Bottom Right
            Vector3f(-0.5f, 0.5f, 0.0f), // Bottom Left
        };

        colors_ = {
            Vector3f(1.0f, 0.0f, 0.0f),  // Red
            Vector3f(0.0f, 1.0f, 0.0f),  // Green
            Vector3f(0.0f, 0.0f, 1.0f),  // Blue
            Vector3f(1.0f, 1.0f, 1.0f),  // White
        };

        indices_ = {
            0, 1, 2, 2, 3, 0
        };
    }

    // -- Swapchain --
    {
        WindowStatues window_statues = Display::get_default().query_window_statues(window_handle_);
        frame_context_.frame_height = static_cast<uint32>(window_statues.height);
        frame_context_.frame_width = static_cast<uint32>(window_statues.width);
        swapchain_ = device_->create_swapchain(frame_context_.frame_width, frame_context_.frame_height);
    }

    // -- Queues --
    {
        const Array<RHICommandQueueRef>& command_queues = device_->get_command_queues();

        RHICommandQueueRef* graphics_command_queue_ptr = command_queues.get_if([](RHICommandQueueRef command_queue) {
            return command_queue->get_type() == RHIQueueType::Graphics; 
        });
        LCHECK_MSG(graphics_command_queue_ptr, "Found no graphics command queue.");
        graphics_command_queue_ = *graphics_command_queue_ptr;

        RHICommandQueueRef* graphics_present_command_queue_ptr = command_queues.get_if([](RHICommandQueueRef command_queue) { 
            return command_queue->get_type() == RHIQueueType::Graphics && command_queue->is_present_mode(); 
        });
        LCHECK_MSG(graphics_present_command_queue_ptr, "Found no graphics command queue that support the present mode.");
        graphics_present_command_queue_ = *graphics_present_command_queue_ptr;
    }

    // -- Render Pass --
    {
        RHIAttachmentDescription render_pass_color_attachment = {};
        render_pass_color_attachment.format = swapchain_->get_format();

        RHIRenderPassDescription render_pass_description = {};
        render_pass_description.attachment_decriptions = {render_pass_color_attachment};

        render_pass_ = device_->create_render_pass(render_pass_description);
    }

    // -- Descriptor set layouts --
    {
        RHIDescriptorSetLayoutBinding binding = {};
        binding.binding = 0;
        binding.count = 1;
        binding.type = RHIDescriptorSetType::Uniform;

        uniform_layout_ = device_->create_descriptor_set({binding});
    }

    // -- Graphics Pipeline --
    {       
        // -- Bindings and attributes --
        RHIVertexBindingDescription position_input_binding_description = {};
        position_input_binding_description.binding = 0;
        position_input_binding_description.stride = sizeof(Vector3f);
        position_input_binding_description.input_rate = RHIVertexInputRate::Vertex;

        RHIVertexBindingDescription color_input_binding_description = {};
        color_input_binding_description.binding = 1;
        color_input_binding_description.stride = sizeof(Vector3f);
        color_input_binding_description.input_rate = RHIVertexInputRate::Vertex;

        RHIVertexAttributeDescription position_attribute_description = {};
        position_attribute_description.binding = 0;
        position_attribute_description.location = 0;
        position_attribute_description.format = RHIFormat::RGB32Float;
        position_attribute_description.offset = 0;

        RHIVertexAttributeDescription color_attribute_description = {};
        color_attribute_description.binding = 1;
        color_attribute_description.location = 1;
        color_attribute_description.format = RHIFormat::RGB32Float;
        color_attribute_description.offset = 0;
        
        RHIPipelineVertexBindingInformation pipeline_vertex_binding_information = {};
        pipeline_vertex_binding_information.bindings = { position_input_binding_description, color_input_binding_description };
        pipeline_vertex_binding_information.attributes = { position_attribute_description, color_attribute_description };

        float32 width = static_cast<float32>(swapchain_->get_width());
        float32 height = static_cast<float32>(swapchain_->get_height());

        // -- Load shaders binary codes --
        FileSystem& file_system = FileSystem::get_platform();

        FileHandleResult vertex_file_open_error = file_system.open_read("shaders/main.vert.spv");
        LCHECK(vertex_file_open_error.has_value())
        SharedRef<FileHandle> vertex_file_handle = vertex_file_open_error.value();
        CompiledShader vertex_shader(vertex_file_handle->read_all_bytes());

        FileHandleResult fragment_file_open_error = file_system.open_read("shaders/main.frag.spv");
        LCHECK(fragment_file_open_error.has_value())
        SharedRef<FileHandle> fragment_file_handle = fragment_file_open_error.value();
        CompiledShader fragment_shader(fragment_file_handle->read_all_bytes());

        // -- Graphics Pipeline Shaders --
        RHIPipelineShaderStageCreateInfo vertex_stage_create_info;
        vertex_stage_create_info.name = "main";
        vertex_stage_create_info.shader = &vertex_shader;
        vertex_stage_create_info.type = RHIShaderStage::Vertex;

        RHIPipelineShaderStageCreateInfo fragment_stage_create_info;
        fragment_stage_create_info.name = "main";
        fragment_stage_create_info.shader = &fragment_shader;
        fragment_stage_create_info.type = RHIShaderStage::Fragment;

        // -- Graphics Pipeline --
        Viewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = width;
        viewport.height = height;
        viewport.min_depth = 0.0f;
        viewport.max_depth = 1.0f;

        Rect2D scissor = {};
        scissor.x = 0.0f;
        scissor.y = 0.0f;
        scissor.width = width;
        scissor.height = height;

        RHIPipelineViewportStateInformation viewport_info;
        viewport_info.viewport = viewport;
        viewport_info.scissor = scissor;

        RHIPipelineDescription pipeline_description;
        pipeline_description.render_pass = render_pass_;
        pipeline_description.vertex_shader_info = vertex_stage_create_info;
        pipeline_description.fragment_shader_info = fragment_stage_create_info;
        pipeline_description.vertex_binding_info = pipeline_vertex_binding_information;
        pipeline_description.viewport_info = viewport_info;
        pipeline_description.descriptor_set_layouts = {uniform_layout_}; 
        pipeline_ = device_->create_graphics_pipeline(pipeline_description);
    }

    // -- Framebuffers --
    {
        framebuffers_.reserve(swapchain_->get_texture_views().size());
        for (RHITextureViewHandle texture : swapchain_->get_texture_views()) {
            RHIFramebufferDescription description = {};
            description.height = swapchain_->get_height();
            description.width = swapchain_->get_width();
            description.render_pass = render_pass_;
            description.attachments = {texture};
            description.layers = 1;

            RHIFramebufferHandle framebuffer = device_->create_framebuffer(render_pass_, description);
            framebuffers_.append(framebuffer);
        }
    }

    // -- Buffers --
    {   
        RHIBufferHandle staging_position_buffer;
        RHIBufferHandle staging_color_buffer;
        RHIBufferHandle staging_index_buffer;

        // Positions
        {
            RHIBufferDescription staging_position_buffer_description = {};
            staging_position_buffer_description.access_mode = RHIAccessMode::Shared;
            staging_position_buffer_description.usage = RHIBufferUsage::TransferSrc;
            staging_position_buffer_description.memory_usage = RHIBufferMemoryUsage::Host;
            staging_position_buffer_description.size = sizeof(Vector3f) * positions_.size();

            staging_position_buffer = device_->create_buffer(staging_position_buffer_description);
            staging_position_buffer->update(positions_.data(), sizeof(Vector3f) * positions_.size());

            RHIBufferDescription position_buffer_description = {};
            position_buffer_description.access_mode = RHIAccessMode::Shared;
            position_buffer_description.usage = RHIBufferUsage::Vertex | RHIBufferUsage::TransferDst;
            position_buffer_description.memory_usage = RHIBufferMemoryUsage::Device;
            position_buffer_description.size = sizeof(Vector3f) * positions_.size();

position_buffer_ = device_->create_buffer(position_buffer_description);
        }

        // Colors
        {
            RHIBufferDescription staging_color_buffer_description = {};
            staging_color_buffer_description.access_mode = RHIAccessMode::Shared;
            staging_color_buffer_description.usage = RHIBufferUsage::TransferSrc;
            staging_color_buffer_description.memory_usage = RHIBufferMemoryUsage::Host;
            staging_color_buffer_description.size = sizeof(Vector3f) * colors_.size();

            staging_color_buffer = device_->create_buffer(staging_color_buffer_description);
            staging_color_buffer->update(colors_.data(), sizeof(Vector3f)* colors_.size());

            RHIBufferDescription color_buffer_description = {};
            color_buffer_description.access_mode = RHIAccessMode::Shared;
            color_buffer_description.usage = RHIBufferUsage::Vertex | RHIBufferUsage::TransferDst;
            color_buffer_description.memory_usage = RHIBufferMemoryUsage::Device;
            color_buffer_description.size = sizeof(Vector3f) * colors_.size();

            color_buffer_ = device_->create_buffer(color_buffer_description);
        }

        // Indices
        {
            RHIBufferDescription staging_index_buffer_description = {};
            staging_index_buffer_description.access_mode = RHIAccessMode::Shared;
            staging_index_buffer_description.usage = RHIBufferUsage::TransferSrc;
            staging_index_buffer_description.memory_usage = RHIBufferMemoryUsage::Host;
            staging_index_buffer_description.size = sizeof(uint32) * indices_.size();

            staging_index_buffer = device_->create_buffer(staging_index_buffer_description);
            staging_index_buffer->update(indices_.data(), sizeof(uint32) * indices_.size());

            RHIBufferDescription index_buffer_description = {};
            index_buffer_description.access_mode = RHIAccessMode::Shared;
            index_buffer_description.usage = RHIBufferUsage::Index | RHIBufferUsage::TransferDst;
            index_buffer_description.memory_usage = RHIBufferMemoryUsage::Device;
            index_buffer_description.size = sizeof(uint32) * indices_.size();

            index_buffer_ = device_->create_buffer(index_buffer_description);
        }

        // -- Upload Data from Standing Buffers to Device Buffers --
        {
            // Fetch transfer queue.
            const Array<RHICommandQueueRef>& command_queues = device_->get_command_queues();
            RHICommandQueueRef* transfer_queue_ptr = command_queues.get_if([](RHICommandQueueRef queue) {
                return queue->get_type() == RHIQueueType::Transfer;
                });
            LCHECK_MSG(transfer_queue_ptr, "Found no transfer command queue.");
            RHICommandQueueRef transfer_queue = *transfer_queue_ptr;

            RHICommandAllocatorDescription transfer_command_allocator_desc = {};
            transfer_command_allocator_desc.count = 1;  // One command buffer allocated.
            transfer_command_allocator_desc.command_queue = transfer_queue;

            RHICommandAllocatorRef transfer_command_allocator_ = device_->create_command_allocator(transfer_command_allocator_desc);

            // Open a transfer command buffer
            RHICommandBufferHandle transfer_cmd = transfer_command_allocator_->open();
            transfer_command_allocator_->reset_command_buffer(transfer_cmd);

            transfer_cmd->begin();
            {
                // Copy positions
                RHIBufferCopyCommand position_copy = {};
                position_copy.size = sizeof(Vector3f) * positions_.size();
                transfer_cmd->copy_buffer(staging_position_buffer, position_buffer_, position_copy);

                // Copy colors
                RHIBufferCopyCommand color_copy = {};
                color_copy.size = sizeof(Vector3f) * colors_.size();
                transfer_cmd->copy_buffer(staging_color_buffer, color_buffer_, color_copy);

                // Copy indices
                RHIBufferCopyCommand indices_copy = {};
                indices_copy.size = sizeof(uint32) * indices_.size();
                transfer_cmd->copy_buffer(staging_index_buffer, index_buffer_, indices_copy);
            }
            transfer_cmd->end();

            // Submit and wait for transfer to complete
            RHIFenceHandle upload_fence = device_->create_fence();
            device_->reset_fence(upload_fence);
            transfer_queue->submit({ transfer_cmd }, {}, {}, upload_fence);

            device_->wait_fence(upload_fence);
            device_->destroy_fence(upload_fence);

            // Standing buffers no longer needed after data upload
            device_->destroy_buffer(staging_position_buffer);
            device_->destroy_buffer(staging_color_buffer);
            device_->destroy_buffer(staging_index_buffer);

            device_->destroy_command_allocator(transfer_command_allocator_);
        }
    }

    // -- Uniform Buffers --
    {
        uniform_buffers_.reserve(swapchain_->get_texture_views().size());
        for (size_t i = 0; i < swapchain_->get_texture_views().size(); i++) {
            RHIBufferDescription uniform_buffer_description = {};
            uniform_buffer_description.access_mode = RHIAccessMode::Private;
            uniform_buffer_description.usage = RHIBufferUsage::Uniform;
            uniform_buffer_description.memory_usage = RHIBufferMemoryUsage::Host;
            uniform_buffer_description.size = sizeof(UniformBufferObject);

            RHIBufferHandle uniform_buffer = device_->create_buffer(uniform_buffer_description);
            uniform_buffers_.append(uniform_buffer);
        }
    }

    // -- Command Allocators
    {
        RHICommandAllocatorDescription graphics_command_allocator_desc = {};
        graphics_command_allocator_desc.count = frame_context_.frame_count;
        graphics_command_allocator_desc.command_queue = graphics_command_queue_; 

        graphics_command_allocator_ = device_->create_command_allocator(graphics_command_allocator_desc);
    }

    // -- Frame Context Sync --
    {
        frame_context_.frame_available_semaphores.reserve(frame_context_.frame_count);
        frame_context_.render_finished_semaphores.reserve(frame_context_.frame_count);
        frame_context_.in_flight_fences.reserve(frame_context_.frame_count);
        frame_context_.frame_in_flight_fences.reserve(swapchain_->get_texture_views().size());

        for (uint32 i = 0; i < frame_context_.frame_count; i++) {
            frame_context_.frame_available_semaphores.append(device_->create_semaphore());
            frame_context_.render_finished_semaphores.append(device_->create_semaphore());
            frame_context_.in_flight_fences.append(device_->create_fence());
        }

        for (uint32 i = 0; i < swapchain_->get_texture_views().size(); i++) {
            frame_context_.frame_in_flight_fences.append(nullptr);
        }
    }
}

void RenderFrameScript::on_tick(float32 delta_time) {
    if (pause_) {
        return;
    }

    swapchain_->acquire_next_frame(frame_context_);

    if (frame_context_.out_of_date) {
        reset();
        return;
    }
    LCHECK(frame_context_.success);

    RHICommandBufferHandle graphics_command_buffer = graphics_command_allocator_->open(frame_context_.current_frame);
    graphics_command_allocator_->reset_command_buffer(graphics_command_buffer);

    float32 width = static_cast<float32>(swapchain_->get_width());
    float32 height = static_cast<float32>(swapchain_->get_height());

    graphics_command_buffer->begin();
    {
        Rect2D render_pass_area = {};
        render_pass_area.x = 0.0f;
        render_pass_area.y = 0.0f;
        render_pass_area.width = width;
        render_pass_area.height = height;

        RHIRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.render_pass = render_pass_;
        render_pass_begin_info.framebuffer = framebuffers_[frame_context_.frame_index];
        render_pass_begin_info.area = render_pass_area;
        graphics_command_buffer->begin_render_pass(render_pass_begin_info);
        {
            graphics_command_buffer->bind_pipeline(pipeline_);

            Viewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = width;
            viewport.height = height;
            viewport.min_depth = 0.0f;
            viewport.max_depth = 1.0f;

            graphics_command_buffer->set_viewports(&viewport, 1);

            Rect2D scissor = {};
            scissor.x = 0.0f;
            scissor.y = 0.0f;
            scissor.width = width;
            scissor.height = height;

            graphics_command_buffer->set_scissors(&scissor, 1);

            graphics_command_buffer->bind_vertex_buffers({position_buffer_, color_buffer_});
            graphics_command_buffer->bind_index_buffer(index_buffer_);    

            RHIDrawIndexedCommand draw_indexed_command = {};
            draw_indexed_command.index_count = indices_.size();
            draw_indexed_command.instance_count = 1;

            graphics_command_buffer->draw(draw_indexed_command);
        }
        graphics_command_buffer->end_render_pass();
    }
    graphics_command_buffer->end();

    if (RHIFenceHandle* fence = frame_context_.frame_in_flight_fences[frame_context_.frame_index]) {
        device_->wait_fence(*fence);
    }

    frame_context_.frame_in_flight_fences[frame_context_.frame_index] = &frame_context_.in_flight_fences[frame_context_.current_frame];

    device_->reset_fence(frame_context_.in_flight_fences[frame_context_.current_frame]);
    
    update_uniform();

    graphics_command_queue_->submit({graphics_command_buffer}, 
        {frame_context_.current_frame_available_semaphore()}, 
        {frame_context_.current_render_finished_semaphore()}, 
        frame_context_.current_in_flight_fence());

    graphics_present_command_queue_->present(swapchain_, frame_context_);

    if (window_resized_ || frame_context_.suboptimal || frame_context_.out_of_date) {
        reset();
        window_resized_ = false;
    }

    LCHECK(frame_context_.success);
    device_->wait_fence(frame_context_.in_flight_fences[frame_context_.current_frame]);

    frame_context_.next_frame();
}

void RenderFrameScript::update_uniform() {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float32 time = std::chrono::duration<float32, std::chrono::seconds::period>(currentTime - startTime).count();
    
    float32 aspect_ratio = swapchain_->get_width() / static_cast<float32>(swapchain_->get_height());
    UniformBufferObject ubo = {};
    ubo.model = Matrix4f::rotate(Matrix4f(1.0f), time * radians(90.0f), Vector3f(0.0f, 0.0f, 1.0f));
    ubo.view = Matrix4f::look_at(Vector3f(2.0f, 2.0f, 2.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
    ubo.proj = Matrix4f::perspective(radians(45.0f), aspect_ratio, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1.0f;
    
    uniform_buffers_[frame_context_.current_frame]->update(&ubo, sizeof(UniformBufferObject), 0);
}

void RenderFrameScript::reset() {
    device_->wait_idle();

    // Destroy all existing framebuffers
    for (RHIFramebufferHandle framebuffer : framebuffers_) {
        device_->destroy_framebuffer(framebuffer);
    }
    framebuffers_.clear();
    framebuffer_memory_allocator_.reset();

    // Recreate swapchain
    device_->recreate_swapchain(swapchain_, frame_context_.frame_width, frame_context_.frame_height);

    // Create new framebuffers
    framebuffers_.reserve(swapchain_->get_texture_views().size());
    for (RHITextureViewHandle texture : swapchain_->get_texture_views()) {
        RHIFramebufferDescription description = {};
        description.height = swapchain_->get_height();
        description.width = swapchain_->get_width();
        description.render_pass = render_pass_;
        description.attachments = {texture};
        description.layers = 1;

        RHIFramebufferHandle framebuffer = device_->create_framebuffer(render_pass_, description);
        framebuffers_.append(framebuffer);
    }
}

void RenderFrameScript::on_shutdown() {
    device_->wait_idle();

    // -- Frame Context --
    {
        for (uint32 i = 0; i < frame_context_.frame_count; i++) {
            device_->destroy_semaphore(frame_context_.frame_available_semaphores[i]);
            device_->destroy_semaphore(frame_context_.render_finished_semaphores[i]);
            device_->destroy_fence(frame_context_.in_flight_fences[i]);
        }
        frame_context_.frame_available_semaphores.clear();
        frame_context_.render_finished_semaphores.clear();
        frame_context_.in_flight_fences.clear();
        frame_context_.frame_in_flight_fences.clear();
    }

    // -- Buffers --
    {
        device_->destroy_buffer(position_buffer_);
        device_->destroy_buffer(color_buffer_);
        device_->destroy_buffer(index_buffer_);
    }

    // -- Uniform buffers --
    {
        for (size_t i = 0; i < uniform_buffers_.size(); i++) {
            device_->destroy_buffer(uniform_buffers_[i]);
        }
        uniform_buffers_.clear();
        device_->destroy_descriptor_set(uniform_layout_);
    }

    // -- Allocators
    {
        device_->destroy_command_allocator(graphics_command_allocator_);
    }
    
    // -- Framebuffers --
    {
        for (RHIFramebufferHandle framebuffer : framebuffers_) {
            device_->destroy_framebuffer(framebuffer);
        }
        framebuffers_.clear();
        framebuffer_memory_allocator_.reset();
    }
        
    device_->destroy_graphics_pipeline(pipeline_);

    device_->destroy_render_pass(render_pass_);

    device_->destroy_swapchain(swapchain_);
}

void RenderFrameScript::update_resized(const uint32 width, const uint32 height) {
    window_resized_ = true;
    frame_context_.frame_width = width;
    frame_context_.frame_height = height;
}

void RenderFrameScript::pause() {
    pause_ = true;
}

void RenderFrameScript::unpause() {
    pause_ = false;
}

}  // namespace licht