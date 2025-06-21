#include "demo.hpp"

#include <cstdlib>
#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_module.hpp"

namespace licht::demo {

static bool g_is_running = false;

void DemoMessageHandler::on_window_close(WindowHandle p_window) {
    LLOG_INFO("[DemoMessageHandler::on_window_close]", "Window closed.");
    g_is_running = false;
}

void DemoMessageHandler::on_window_resized(WindowHandle p_window, uint32 p_width, uint32 p_height) {
    LLOG_INFO("[DemoMessageHandler::on_window_resized]", vformat("Window resized to %dx%d", p_width, p_height));
}

void DemoMessageHandler::on_mouse_wheel(float32 p_delta) {
    LLOG_INFO("[DemoMessageHandler::on_mouse_wheel]", vformat("Mouse wheel scrolled by %d", p_delta));
}

void DemoMessageHandler::on_key_down(Key p_key) {
    LLOG_INFO("[DemoMessageHandler::on_key_down]", vformat("Key down: %s", key_to_string(p_key)));
}

int32 launch(int32 p_argc, const char** p_argv) {

    LLOG_INFO("[main]", "Launch application.");

    platform_start();

    Display& display = Display::get_default();
    SharedRef<DemoMessageHandler> message_handler = new_ref<DemoMessageHandler>();
    display.set_message_handler(message_handler);

    WindowHandle window_handle = display.create_window_handle({"Demo Window", 800, 600, 100, 100});
    LCHECK(window_handle == Display::MAIN_WINDOW_HANDLE)

    RHIVulkanModule rhi_module;
    rhi_module.initialize();
    
    display.show(window_handle);

    g_is_running = true;
    while (g_is_running) {
        display.handle_events();
        rhi_module.tick();
    }
        
    rhi_module.shutdown();

    platform_end();

    return EXIT_SUCCESS;
}

}

int32 main(int32 p_argc, const char** p_argv) {
    return licht::demo::launch(p_argc, p_argv);
}
