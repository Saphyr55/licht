#include "ludo_message_handler.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/core/signals/signal.hpp"
#include "licht/core/containers/array.hpp"

void LudoDisplayMessageHandler::on_window_close(const WindowHandle window) {
    LLOG_INFO("[LudoDisplayMessageHandler::on_window_close]", "Window closed.");
    ludo_stop_app();
}

void LudoDisplayMessageHandler::on_window_resized(const WindowHandle window, const uint32 width, const uint32 height) {
    LLOG_INFO("[LudoDisplayMessageHandler::on_window_resized]", vformat("Window resized to %dx%d", width, height));
    frame_script_->update_resized(width, height);
}

void LudoDisplayMessageHandler::on_window_minimized(const WindowHandle window) {
    frame_script_->pause();
}

void LudoDisplayMessageHandler::on_window_shown(const WindowHandle window) {
    frame_script_->unpause();
}

void LudoDisplayMessageHandler::on_mouse_wheel(const float32 delta) {
    Input::on_mouse_wheel.emit(MouseWheel(delta));
}

void LudoDisplayMessageHandler::on_mouse_move(float32 pos_rel_x, float32 pos_rel_y, float32 pos_x, float32 pos_y) {
    Input::on_mouse_move.emit(MouseMove(pos_rel_x, pos_rel_y, pos_x, pos_y));
}

void LudoDisplayMessageHandler::on_button_up(Button button) {
    Input::buttons_pressed_.remove(button);
    Input::buttons_dow_.remove(button);

    if (!Input::buttons_release_.contains(button)) {
        Input::buttons_release_.append(button);
        Input::on_button_release.emit(button);
    }

    Input::buttons_up_.insert(button);
    Input::on_button_up(button);
}

void LudoDisplayMessageHandler::on_button_down(const Button button) {
    Input::buttons_release_.remove(button);
    Input::buttons_up_.remove(button);

    if (!Input::buttons_pressed_.contains(button)) {
        Input::buttons_pressed_.append(button);
        Input::on_button_pressed(button);
    }

    Input::buttons_dow_.insert(button);
    Input::on_button_down(button);
}

void LudoDisplayMessageHandler::on_key_down(const VirtualKey key) {
    Input::keys_release_.remove(key);
    Input::keys_up_.remove(key);

    if (!Input::keys_pressed_.contains(key)) {
        Input::keys_pressed_.append(key);
        Input::on_key_pressed(key);
    }

    Input::keys_dow_.insert(key);
    Input::on_key_down(key);
}

void LudoDisplayMessageHandler::on_key_up(const VirtualKey key) {
    Input::keys_pressed_.remove(key);
    Input::keys_dow_.remove(key);

    if (!Input::keys_release_.contains(key)) {
        Input::keys_release_.append(key);
        Input::on_key_release.emit(key);
    }

    Input::keys_up_.insert(key);
    Input::on_key_up(key);
}
