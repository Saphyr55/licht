#include "ludo_module.hpp"
#include "licht/core/platform/input.hpp"
#include "ludo_message_handler.hpp"
#include "render_frame_script.hpp"
#include "camera.hpp"

#include <licht/core/defines.hpp>
#include <licht/core/memory/shared_ref.hpp>
#include <licht/core/modules/module_manifest.hpp>
#include <licht/core/modules/module_registry.hpp>
#include <licht/core/platform/display.hpp>
#include <licht/core/platform/platform.hpp>
#include <licht/core/platform/window_handle.hpp>
#include <licht/core/trace/trace.hpp>
#include <licht/rhi/rhi_module.hpp>

void LudoModule::on_load() {
    LLOG_INFO("[LudoModule]", "Ludo module loaded.");
}

void LudoModule::on_startup() {
    LLOG_INFO("[LudoModule]", "Starting Ludo application.");
}

void LudoModule::on_shutdown() {
    LLOG_INFO("[LudoModule]", "Shutting down Ludo application.");
}

void LudoModule::on_unload() {
    LLOG_INFO("[LudoModule]", "Ludo module unloaded.");
}

int32 ludo_application_launch(int32 argc, const char** argv) {
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

    Camera camera(Vector3f(0.0f, 0.0f, -2.0f));
    camera.look_at(Vector3f(0.0f));

    Input::on_mouse_move.connect([&camera](const MouseMove& mouse_move) -> void {
        if (Input::button_is_down(Button::Left)) {
            camera.look_around(mouse_move.pos_rel_x, mouse_move.pos_rel_y);
        }
    });

    RenderFrameScript render_frame_script(&camera);
    render_frame_script.on_startup();
    
    // By creating a DisplayMessageHandler, you can intercept the platform and window events.
    SharedRef<DemoMessageHandler> demo_message_handler = new_ref<DemoMessageHandler>();
    display.set_message_handler(demo_message_handler);
    demo_message_handler->set_render_frame_script(&render_frame_script);

    // Show the window.
    display.show(window_handle);

    // We use global variable for the loop.
    // TODO: Must externalize the loop.
    g_is_running = true;

    // Main loop
    while (g_is_running) {
        // Handle window and platform events
        display.handle_events(); 

        camera_update_position(camera);

        // Tick the render frame script with a fixed delta time (0.0f for now)
        render_frame_script.on_tick(0.0f);
    }

    // Do not forget to stop it.
    // TODO: Need to be done by a manager
    render_frame_script.on_shutdown();
    rhi_module->on_shutdown();
    
    return EXIT_SUCCESS;
}
