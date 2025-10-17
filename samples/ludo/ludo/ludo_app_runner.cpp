#include "ludo_app_runner.hpp"

#include "ludo_message_handler.hpp"
#include "render_frame_script.hpp"

#include <licht/core/defines.hpp>
#include <licht/core/memory/shared_ref.hpp>
#include <licht/core/modules/module_manifest.hpp>
#include <licht/core/modules/module_registry.hpp>
#include <licht/core/platform/display.hpp>
#include <licht/core/platform/input.hpp>
#include <licht/core/platform/platform.hpp>
#include <licht/core/platform/window_handle.hpp>
#include <licht/core/time/delta_timer.hpp>
#include <licht/core/time/frame_rate_monitor.hpp>
#include <licht/core/trace/trace.hpp>
#include <licht/rhi/rhi_module.hpp>
#include <licht/scene/camera.hpp>

constexpr float64 TargetFPS = 144.0;
constexpr float64 TargetFrameRate = 1.0 / TargetFPS;

static Camera initial_camera = []() -> Camera&& {
    Camera camera(Vector3f(0.0f, 5.0f, 0.0f));
    camera.look_at(Vector3f(0.0f));
    camera.movement_speed = 10.0f;
    return std::move(camera);
}();

void LudoAppRunner::camera_on_tick(Camera& camera, float64 delta_time) {
    Vector3f direction(0.0f, 0.0f, 0.0f);

    if (Input::key_is_down(VirtualKey::Z)) {
        direction += camera.front;
    }

    if (Input::key_is_down(VirtualKey::S)) {
        direction -= camera.front;
    }

    if (Input::key_is_down(VirtualKey::Q)) {
        direction -= camera.right;
    }

    if (Input::key_is_down(VirtualKey::D)) {
        direction += camera.right;
    }

    if (Input::key_is_down(VirtualKey::Space)) {
        direction += camera.world_up;
    }

    if (Input::key_is_down(VirtualKey::LeftShift)) {
        direction -= camera.world_up;
    }

    if (Vector3f::length(direction) > 0.0f) {
        direction = Vector3f::normalize(direction);
        camera.position += direction * camera.movement_speed * delta_time;
        camera.update_vectors();
    }

    camera.update_view();
}

void LudoAppRunner::on_run_delegate() {
    // Create a window. WindowHandle behave like a reference.
    Display& display = Display::get_default();
    const WindowStatues window_statues("Demo Window", 800, 600, 100, 100);
    const WindowHandle window_handle = display.create_window_handle(window_statues);

    // Module Registry manages the module.
    ModuleRegistry& module_registry = ModuleRegistry::get_instance();
    RHIModule* rhi_module = module_registry.get_module<RHIModule>(RHIModule::ModuleName);

    // Need to know the window for creating the surface.
    rhi_module->set_window_handle(window_handle);

    // Vulkan API is already the default option.
    rhi_module->set_graphics_api(GraphicsAPI::Vulkan);

    // Each module must be started, but do not forget to stop it by calling the on_shutdown() method,
    // otherwise you may experience some memory leaks.
    // TODO: Startup and shutdown must be launched automatically after refactoring the engine loop.
    rhi_module->on_startup();

    Camera camera = initial_camera;

    Input::on_mouse_move.connect([&camera](const MouseMove& mouse_move) -> void {
        if (Input::button_is_down(Button::Left)) {
            camera.look_around(mouse_move.pos_rel_x, -mouse_move.pos_rel_y);
        }
    });

    RenderFrameScript render_frame_script(&camera);
    render_frame_script.on_startup();

    // By creating a DisplayMessageHandler, you can intercept the platform and window events.
    SharedRef<LudoDisplayMessageHandler> demo_message_handler = new_ref<LudoDisplayMessageHandler>();
    display.set_message_handler(demo_message_handler);
    demo_message_handler->set_render_frame_script(&render_frame_script);

    // Show the window.
    display.show(window_handle);

    // We use global variable for the loop.
    // TODO: Must externalize the loop.
    g_is_app_running = true;

    DeltaTimer timer;
    FrameRateMonitor frame_monitor;

    bool unlimited_frame_rate = false;

    // Main loop
    while (g_is_app_running) {
        float64 delta_time = timer.tick();

        // Frame rate limiting
        if (!unlimited_frame_rate) {
            delta_time = timer.limit(delta_time, TargetFrameRate);
        }

        // Handle window and platform events
        display.handle_events();

        // Update the camera, must be call once per frame.
        camera_on_tick(camera, delta_time);

        // Tick the render frame script with a delta time
        render_frame_script.on_tick(delta_time);

        // Restart the initial camera state.
        if (Input::key_is_pressed(VirtualKey::C)) {
            camera = initial_camera;
        }

        // Switch between 144Hz and unlimited frame rate.
        if (Input::key_is_pressed(VirtualKey::F1)) {
            unlimited_frame_rate = !unlimited_frame_rate;
        }

        // Restart application when Ctrl+R key pressed.
        if (Input::key_is_pressed(VirtualKey::R) && Input::key_is_down(VirtualKey::LeftControl)) {
            ludo_restart_app();
        }

        // Shutdown engine and application when Escape key pressed.
        if (Input::key_is_pressed(VirtualKey::Escape)) {
            ludo_stop_app();
        }
    }

    // Do not forget to stop it.
    // TODO: Need to be done by a manager
    render_frame_script.on_shutdown();
    rhi_module->on_shutdown();

    display.close(window_handle);
}

int32 LudoAppRunner::run() {
    g_is_engine_running = true;
    while (g_is_engine_running) {
        on_run_delegate();
    }
    return EXIT_SUCCESS;
}