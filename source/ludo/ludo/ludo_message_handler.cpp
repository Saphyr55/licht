#include "ludo_message_handler.hpp"
#include "licht/core/trace/trace.hpp"

void DemoMessageHandler::on_window_close(const WindowHandle window) {
    LLOG_INFO("[DemoMessageHandler::on_window_close]", "Window closed.");
    g_is_running = false;
}

void DemoMessageHandler::on_window_resized(const WindowHandle window, const uint32 width, const uint32 height) {
    LLOG_INFO("[DemoMessageHandler::on_window_resized]", vformat("Window resized to %dx%d", width, height));
    rhi_module_->update_resized(width, height);
}

void DemoMessageHandler::on_window_minimized(const WindowHandle window) {
    rhi_module_->pause();
}

void DemoMessageHandler::on_window_shown(const WindowHandle window) {
    rhi_module_->unpause();
}

void DemoMessageHandler::on_mouse_wheel(const float32 delta) {
    LLOG_INFO("[DemoMessageHandler::on_mouse_wheel]", vformat("Mouse wheel scrolled by %d", delta));
}

void DemoMessageHandler::on_key_down(const Key key) {
    LLOG_INFO("[DemoMessageHandler::on_key_down]", vformat("Key down: %s", key_to_string(key)));
}

