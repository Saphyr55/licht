#include "ludo_message_handler.hpp"
#include "licht/core/trace/trace.hpp"

void DemoMessageHandler::on_window_close(const WindowHandle window) {
    LLOG_INFO("[DemoMessageHandler::on_window_close]", "Window closed.");
    g_is_running = false;
}

void DemoMessageHandler::on_window_resized(const WindowHandle window, const uint32 width, const uint32 height) {
    LLOG_INFO("[DemoMessageHandler::on_window_resized]", vformat("Window resized to %dx%d", width, height));
    frame_script_->update_resized(width, height);
}

void DemoMessageHandler::on_window_minimized(const WindowHandle window) {
    frame_script_->pause();
}

void DemoMessageHandler::on_window_shown(const WindowHandle window) {
    frame_script_->unpause();
}

void DemoMessageHandler::on_mouse_wheel(const float32 delta) {
    Input::on_mouse_wheel.emit(MouseWheel(delta));
}

void DemoMessageHandler::on_mouse_move(float32 pos_rel_x, float32 pos_rel_y, float32 pos_x, float32 pos_y) {
    Input::on_mouse_move.emit(MouseMove(pos_rel_x, pos_rel_y, pos_x, pos_y));
}

void DemoMessageHandler::on_button_up(Button button) {
    Input::buttons_pressed_.remove(button);
    Input::buttons_dow_.erase(button);

    if (!Input::buttons_release_.contains(button)) {
        Input::buttons_release_.append(button);
        Input::on_button_release.emit(button);
    }

    Input::buttons_up_.insert(button);
    Input::on_button_up(button);
}

void DemoMessageHandler::on_button_down(const Button button) {
    Input::buttons_release_.remove(button);
    Input::buttons_up_.erase(button);

    if (!Input::buttons_pressed_.contains(button)) {
        Input::buttons_pressed_.append(button);
        Input::on_button_pressed(button);
    }

    Input::buttons_dow_.insert(button);
    Input::on_button_down(button);
}

void DemoMessageHandler::on_key_down(const VirtualKey key) {
    Input::keys_release_.remove(key);
    Input::keys_up_.erase(key);

    if (!Input::keys_pressed_.contains(key)) {
        Input::keys_pressed_.append(key);
        Input::on_key_pressed(key);
    }

    Input::keys_dow_.insert(key);
    Input::on_key_down(key);
}

void DemoMessageHandler::on_key_up(const VirtualKey key) {
    Input::keys_pressed_.remove(key);
    Input::keys_dow_.erase(key);

    if (!Input::keys_release_.contains(key)) {
        Input::keys_release_.append(key);
        Input::on_key_release.emit(key);
    }

    Input::keys_up_.insert(key);
    Input::on_key_up(key);
}
