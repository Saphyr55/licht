#include "render_frame_script.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/io/file_handle.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/math/common_math.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/display.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/rhi/buffer.hpp"
#include "licht/rhi/command_queue.hpp"
#include "licht/rhi/descriptor_set.hpp"
#include "licht/rhi/rhi_fowards.hpp"
#include "licht/rhi/rhi_module.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "ludo_types.hpp"


#include <chrono>

namespace licht {

RenderFrameScript::RenderFrameScript()
    : window_handle_(Display::InvalidWindowHandle)
    , device_(nullptr)
    , swapchain_(nullptr)
    , render_pass_(nullptr)
    , graphics_pipeline_(nullptr)
    , framebuffer_memory_allocator_(1024 /* 1 kB */)
    , framebuffers_(4, RHIFramebufferAllocator(framebuffer_memory_allocator_))
    , graphics_command_allocator_(nullptr)
    , frame_context_()
    , pause_(false)
    , window_resized_(false) {
}

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
            Vector3f(-0.5f, -0.5f, 0.0f),  // Top Left
            Vector3f(0.5f, -0.5f, 0.0f),   // Top Right
            Vector3f(0.5f, 0.5f, 0.0f),    // Bottom Right
            Vector3f(-0.5f, 0.5f, 0.0f),   // Bottom Left
        };

        colors_ = {
            Vector3f(1.0f, 0.0f, 0.0f),  // Red
            Vector3f(0.0f, 1.0f, 0.0f),  // Green
            Vector3f(0.0f, 0.0f, 1.0f),  // Blue
            Vector3f(1.0f, 1.0f, 1.0f),  // White
        };

        indices_ = {
            0, 1, 2, 2, 3, 0};
    }

    // -- Swapchain --
    {
        WindowStatues window_statues = Display::get_default().query_window_statues(window_handle_);
        frame_context_.frame_height = static_cast<uint32>(window_statues.height);
        frame_context_.frame_width = static_cast<uint32>(window_statues.width);
        swapchain_ = device_->create_swapchain(frame_context_.frame_width, frame_context_.frame_height, frame_context_.frame_count);
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
        pipeline_vertex_binding_information.bindings = {position_input_binding_description, color_input_binding_description};
        pipeline_vertex_binding_information.attributes = {position_attribute_description, color_attribute_description};

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

        RHIDescriptorSetLayoutBinding ubo_binding = {};
        ubo_binding.binding = 0;
        ubo_binding.count = 1;
        ubo_binding.type = RHIDescriptorSetType::Uniform;

        RHIPipelineDescription pipeline_description;
        pipeline_description.render_pass = render_pass_;
        pipeline_description.vertex_shader_info = vertex_stage_create_info;
        pipeline_description.fragment_shader_info = fragment_stage_create_info;
        pipeline_description.vertex_binding_info = pipeline_vertex_binding_information;
        pipeline_description.viewport_info = viewport_info;
        pipeline_description.bindings = {ubo_binding};
        graphics_pipeline_ = device_->create_graphics_pipeline(pipeline_description);
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

    // -- Device Buffers --
    {
        RHIDeviceMemoryUploader uploader(device_, 3);

        position_buffer_ = uploader.send_buffer(RHIStagingBufferContext(RHIBufferUsage::Vertex, positions_));
        color_buffer_ = uploader.send_buffer(RHIStagingBufferContext(RHIBufferUsage::Vertex, colors_));
        index_buffer_ = uploader.send_buffer(RHIStagingBufferContext(RHIBufferUsage::Index, indices_));

        uploader.upload();
    }

    // -- Uniform Buffers --
    {
        const uint32 image_count = swapchain_->get_texture_views().size();

        uniform_buffers_.reserve(image_count);
        for (uint32 i = 0; i < image_count; i++) {
            RHIBufferDescription uniform_buffer_description = {};
            uniform_buffer_description.access_mode = RHIAccessMode::Private;
            uniform_buffer_description.usage = RHIBufferUsage::Uniform;
            uniform_buffer_description.memory_usage = RHIBufferMemoryUsage::Host;
            uniform_buffer_description.size = sizeof(UniformBufferObject);

            RHIBufferHandle uniform_buffer = device_->create_buffer(uniform_buffer_description);
            uniform_buffers_.append(uniform_buffer);
        }
    }

    // -- Descriptor set layouts --
    {
        const uint32 image_count = swapchain_->get_texture_views().size();
        constexpr uint32 binding = 0;
        constexpr uint32 offset = 0;
        constexpr uint32 size = sizeof(UniformBufferObject);

        descriptor_pool_ = device_->create_descriptor_pool(graphics_pipeline_, RHIDescriptorSetInformation(image_count));

        for (uint32 i = 0; i < image_count; i++) {
            RHIDescriptorSetRef descriptor_set = descriptor_pool_->get_descriptor_set(i);
            descriptor_set->update(uniform_buffers_[i], binding, offset, size);
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

    graphics_command_buffer->begin();
    {
        float32 width = static_cast<float32>(swapchain_->get_width());
        float32 height = static_cast<float32>(swapchain_->get_height());

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
            graphics_command_buffer->bind_pipeline(graphics_pipeline_);

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

            RHIDescriptorSetRef descriptor_set = descriptor_pool_->get_descriptor_set(frame_context_.current_frame);
            graphics_command_buffer->bind_descriptor_sets(graphics_pipeline_, {descriptor_set});

            RHIDrawIndexedCommand draw_indexed_command = {};
            draw_indexed_command.index_count = indices_.size();
            draw_indexed_command.instance_count = 1;

            graphics_command_buffer->draw(draw_indexed_command);
        }
        graphics_command_buffer->end_render_pass();
    }
    graphics_command_buffer->end();

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
    namespace ch = std::chrono;

    static ch::time_point start_time = ch::high_resolution_clock::now();

    ch::time_point current_time = ch::high_resolution_clock::now();
    float32 time = ch::duration<float32, ch::seconds::period>(current_time - start_time).count();

    float32 aspect_ratio = swapchain_->get_width() / static_cast<float32>(swapchain_->get_height());

    UniformBufferObject ubo;
    ubo.model = Matrix4f::rotate(ubo.model, time * radians(90.0f), Vector3f(0.0f, 0.0f, 1.0f));
    ubo.view = Matrix4f::look_at(Vector3f(2.0f, 2.0f, 2.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
    ubo.proj = Matrix4f::perspective(radians(45.0f), aspect_ratio, 0.0f, 10.0f);
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
        device_->destroy_descriptor_pool(descriptor_pool_);
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

    device_->destroy_graphics_pipeline(graphics_pipeline_);

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