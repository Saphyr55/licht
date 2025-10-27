#include "render_frame_script.hpp"

#include "licht/core/containers/array.hpp"
#include "licht/core/containers/index_range.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/math/math.hpp"
#include "licht/core/math/matrix4.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/math/vector4.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/input.hpp"
#include "licht/engine/project_settings.hpp"
#include "licht/renderer/draw_item.hpp"
#include "licht/renderer/mesh/static_mesh.hpp"
#include "licht/renderer/mesh/static_mesh_loader.hpp"
#include "licht/renderer/shader/shader_compiler.hpp"
#include "licht/rhi/command_buffer.hpp"
#include "licht/rhi/device_memory_uploader.hpp"
#include "licht/rhi/graphics_pipeline.hpp"
#include "licht/rhi/rhi_forwards.hpp"
#include "licht/rhi/rhi_module.hpp"
#include "licht/rhi/rhi_types.hpp"
#include "licht/rhi/texture.hpp"
#include "licht/rhi/texture_pool.hpp"
#include "licht/rhi/texture_view.hpp"
#include "licht/scene/camera.hpp"
#include "ludo_types.hpp"
#include "material_graphics_pipeline.hpp"

namespace licht {

RenderFrameScript::RenderFrameScript(Camera* camera, WindowHandle window_handle)
    : window_handle_(window_handle)
    , device_(nullptr)
    , framebuffers_(3)
    , pause_(false)
    , camera_(camera) {
}

void RenderFrameScript::on_startup() {
    ProjectSettings& project_settings = ProjectSettings::get_instance();
    ModuleRegistry& registry = ModuleRegistry::get_instance();
    RHIModule* module = registry.get_module<RHIModule>(RHIModule::ModuleName);
    StringRef projectdir = project_settings.get_name("projectdir");

    FileSystem& file_system = FileSystem::get_platform();

    LCHECK_MSG(compile_shaders(), "Failed to compile shaders.");

    device_ = module->get_device();

    render_context_ = new_ref<RenderContext>();
    material_graphics_pipeline_ = new_ref<MaterialGraphicsPipeline>();

    render_context_->initialize(window_handle_);
    render_context_->on_reset([this]() -> void { reset(); });

    float32 width = render_context_->get_swapchain()->get_width();
    float32 height = render_context_->get_swapchain()->get_height();

    String model_asset_path = projectdir + "/assets/models/Sponza/glTF/Sponza.gltf";

    Array<StaticMesh> meshes_model = gltf_static_meshes_load(model_asset_path);

    depth_texture_ = render_context_->get_texture_pool()->create_texture({
        .format = RHIFormat::D24S8,
        .usage = RHITextureUsageFlags::DepthStencilAttachment,
        .sharing_mode = RHISharingMode::Private,
        .memory_usage = RHIMemoryUsage::Device,
        .width = width,
        .height = height,
    });

    rhi_transition_texture(device_,
                           RHITextureLayoutTransition(depth_texture_,
                                                      RHITextureLayout::Undefined,
                                                      RHITextureLayout::DepthStencilAttachment),
                           render_context_->get_graphics_queue());

    depth_texture_view_ = device_->create_texture_view({
        .texture = depth_texture_,
        .format = depth_texture_->get_description().format,
    });

    material_graphics_pipeline_->initialize(device_, render_context_);

    framebuffers_.reserve(render_context_->get_swapchain()->get_texture_views().size());
    for (RHITextureView* texture : render_context_->get_swapchain()->get_texture_views()) {
        RHIFramebufferDescription description = {
            .render_pass = material_graphics_pipeline_->get_render_pass(),
            .attachments = {texture, depth_texture_view_},
            .width = width,
            .height = height,
            .layers = 1,
        };

        RHIFramebuffer* framebuffer = device_->create_framebuffer(description);
        framebuffers_.append(framebuffer);
    }

    RHIDeviceMemoryUploader uploader =  render_context_->uploader();

    for (StaticMesh& mesh : meshes_model) {
        for (StaticSubMesh& submesh : mesh.get_submeshes()) {
            DrawItem item = DrawItem::create(device_, uploader, submesh);
            item.model_constant.model = Matrix4f::scale(item.model_constant.model, Vector3f(0.005f));
            packet_.items.append(item);
        }
    }

    punctual_light_ = PunctualLight{
        .position = Vector3f(0.0f, 2.0f, 0.5f),
        .color = Vector3f(1.0f, 0.0f, 0.0f),
    };

    uploader.upload(render_context_->get_graphics_queue());

    material_graphics_pipeline_->initialize_shader_resource_pool(packet_.items.size());
    material_graphics_pipeline_->compile(packet_);

    Input::on_key_release.connect([&](const VirtualKey key) {
        if (key == VirtualKey::G) {
            reload_shaders();
        }
    });
}

void RenderFrameScript::on_tick(float64 delta_time) {
    if (pause_) {
        return;
    }

    render_context_->begin_frame();
    {
        RHICommandBuffer* cmd = render_context_->get_current_command_buffer();
        float32 width = static_cast<float32>(render_context_->get_swapchain()->get_width());
        float32 height = static_cast<float32>(render_context_->get_swapchain()->get_height());
        Rect2D area = {.x = 0.0f,
                       .y = 0.0f,
                       .width = width,
                       .height = height};

        RHIRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.render_pass = material_graphics_pipeline_->get_render_pass();
        render_pass_begin_info.framebuffer = framebuffers_[render_context_->get_frame_index()];
        render_pass_begin_info.area = area;
        render_pass_begin_info.color = Vector4f(0.01f, 0.01f, 0.01f, 1.0f);

        cmd->begin_render_pass(render_pass_begin_info);
        {
            RHIGraphicsPipeline* graphics_pipeline = material_graphics_pipeline_->get_graphics_pipeline_handle();

            cmd->bind_graphics_pipeline(graphics_pipeline);

            Rect2D scissor = {.x = 0.0f,
                              .y = 0.0f,
                              .width = width,
                              .height = height};

            Viewport viewport = {.x = 0.0f,
                                 .y = 0.0f,
                                 .width = width,
                                 .height = height,
                                 .min_depth = 0.0f,
                                 .max_depth = 1.0f};

            cmd->set_viewports(&viewport, 1);
            cmd->set_scissors(&scissor, 1);

            // Render Packet
            for (DrawItem& item : packet_.items) {
                RHIShaderResourceGroup* global_group = material_graphics_pipeline_
                                                           ->get_global_shader_resource_pool()
                                                           ->get_group(render_context_->get_current_frame());

                cmd->bind_shader_resource_group(graphics_pipeline, {global_group}, 0);

                cmd->set_shader_constants(graphics_pipeline, RHIShaderConstants(&item.model_constant,
                                                                                sizeof(RenderModelConstant),
                                                                                0,
                                                                                RHIShaderStage::Vertex));

                RHIShaderResourceGroup* shader_group = item.shader_groups[render_context_->get_current_frame()];

                cmd->bind_vertex_buffers(item.vertex_buffers);
                cmd->bind_index_buffer(item.index_buffer);

                cmd->bind_shader_resource_group(graphics_pipeline, {shader_group}, 1);

                RHIDrawIndexedCommand draw_indexed_command = {};
                draw_indexed_command.index_count = item.index_count;
                draw_indexed_command.instance_count = 1;

                cmd->draw(draw_indexed_command);
            }
        }
        cmd->end_render_pass();

        update_uniform(delta_time);
    }
    render_context_->end_frame();
}

void RenderFrameScript::update_uniform(const float64 delta_time) {
    UniformBufferObject ubo;

    float32 width = render_context_->get_swapchain()->get_width();
    float32 height = render_context_->get_swapchain()->get_height();
    float32 aspect_ratio = width / height;

    ubo.view = camera_->view;

    ubo.proj = Matrix4f::perspective(Math::radians(75.0f), aspect_ratio, 0.1f, 10000.0f);
    ubo.view_proj = ubo.proj * ubo.view;

    ubo.eye_position = camera_->position;

    material_graphics_pipeline_->update(ubo);
    material_graphics_pipeline_->update(punctual_light_);
}

void RenderFrameScript::reset() {
    device_->destroy_texture_view(depth_texture_view_);
    render_context_->get_texture_pool()->destroy_texture(depth_texture_);

    RHITextureDescription depth_texture_desc = {};
    depth_texture_desc.format = RHIFormat::D24S8;
    depth_texture_desc.usage = RHITextureUsageFlags::DepthStencilAttachment;
    depth_texture_desc.sharing_mode = RHISharingMode::Private;
    depth_texture_desc.memory_usage = RHIMemoryUsage::Device;
    depth_texture_desc.width = render_context_->get_swapchain()->get_width();
    depth_texture_desc.height = render_context_->get_swapchain()->get_height();
    depth_texture_ = render_context_->get_texture_pool()->create_texture(depth_texture_desc);

    RHITextureViewDescription depth_texture_view_desc = {};
    depth_texture_view_desc.texture = depth_texture_;
    depth_texture_view_desc.format = depth_texture_->get_description().format;
    depth_texture_view_ = device_->create_texture_view(depth_texture_view_desc);

    // Destroy all existing framebuffers
    for (RHIFramebuffer* framebuffer : framebuffers_) {
        device_->destroy_framebuffer(framebuffer);
    }
    framebuffers_.clear();

    // Create new framebuffers
    framebuffers_.reserve(render_context_->get_swapchain()->get_texture_views().size());
    for (RHITextureView* texture_view : render_context_->get_swapchain()->get_texture_views()) {
        RHIFramebufferDescription description = {};
        description.height = render_context_->get_swapchain()->get_height();
        description.width = render_context_->get_swapchain()->get_width();
        description.render_pass = material_graphics_pipeline_->get_render_pass();
        description.attachments = {texture_view, depth_texture_view_};
        description.layers = 1;

        RHIFramebuffer* framebuffer = device_->create_framebuffer(description);
        framebuffers_.append(framebuffer);
    }
}

void RenderFrameScript::on_shutdown() {
    render_context_->shutdown();

    for (RHIFramebuffer* framebuffer : framebuffers_) {
        device_->destroy_framebuffer(framebuffer);
    }
    framebuffers_.clear();

    material_graphics_pipeline_->destroy();

    device_->destroy_texture_view(depth_texture_view_);

    for (DrawItem& item : packet_.items) {
        for (size_t i : IndexRange(0, item.samplers.size())) {
            if (item.samplers[i]) {
                device_->destroy_sampler(item.samplers[i]);
                device_->destroy_texture_view(item.texture_views[i]);
            }
        }
    }

    device_->destroy_swapchain(render_context_->get_swapchain());
}

void RenderFrameScript::reload_shaders() {
    device_->wait_idle();

    compile_shaders();
    material_graphics_pipeline_->reload();
}

void RenderFrameScript::update_resized(const uint32 width, const uint32 height) {
    render_context_->update_resized(width, height);
}

bool RenderFrameScript::compile_shaders() {
    StringRef projectdir = ProjectSettings::get_instance().get_name("projectdir");
    FileSystem& file_system = FileSystem::get_platform();

    String vertex_shader_path = projectdir + "/assets/shaders/ludo.material.vert";
    String fragment_shader_path = projectdir + "/assets/shaders/ludo.material.frag";

    bool vert_ok = SPIRVShaderCompiler::compile_glsl_file(vertex_shader_path, 
        "shaders/ludo.material.vert.spv", SPIRVShaderCompiler::Stage::Vertex);

    bool frag_ok = SPIRVShaderCompiler::compile_glsl_file(fragment_shader_path, 
        "shaders/ludo.material.frag.spv", SPIRVShaderCompiler::Stage::Fragment);

    return vert_ok && frag_ok;
}

void RenderFrameScript::pause() {
    pause_ = true;
}

void RenderFrameScript::unpause() {
    pause_ = false;
}

}  // namespace licht