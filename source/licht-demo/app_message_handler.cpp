#include "app_message_handler.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {

void DemoMessageHandler::on_window_close(WindowHandle window) {
    LLOG_INFO("[DemoMessageHandler::on_window_close]", "Window closed.");
    g_is_running = false;
}

void DemoMessageHandler::on_window_resized(WindowHandle window, uint32 width, uint32 height) {
    LLOG_INFO("[DemoMessageHandler::on_window_resized]", vformat("Window resized to %dx%d", width, height));
    rhi_module_->update_resized(width, height);
}

void DemoMessageHandler::on_window_minized(WindowHandle window) {
    rhi_module_->pause();
}

void DemoMessageHandler::on_window_shown(WindowHandle handle) {
    rhi_module_->unpause();
}

void DemoMessageHandler::on_mouse_wheel(float32 p_delta) {
    LLOG_INFO("[DemoMessageHandler::on_mouse_wheel]", vformat("Mouse wheel scrolled by %d", p_delta));
}

void DemoMessageHandler::on_key_down(Key p_key) {
    LLOG_INFO("[DemoMessageHandler::on_key_down]", vformat("Key down: %s", key_to_string(p_key)));
}

}  //namespace licht