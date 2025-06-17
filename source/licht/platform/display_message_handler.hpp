#pragma once

#include "licht/platform/platform_exports.hpp"
#include "licht/platform/input.hpp"
#include "licht/platform/window_handle.hpp"

namespace licht {

class LICHT_PLATFORM_API DisplayMessageHandler {
public:
    virtual void on_window_close(WindowHandle p_window) {
    }

    virtual void on_window_resized(WindowHandle p_window, uint32 p_width, uint32 p_height) {
    }

    virtual void on_mouse_wheel(float32 p_delta) {
    }

    virtual void on_key_up(Key p_key) {
    }

    virtual void on_key_down(Key p_key) {
    }

    virtual void on_button_up(Button p_button) {
    }

    virtual void on_button_down(Button p_button) {
    }

    virtual void on_mouse_move(float32 p_pos_rel_x, float32 p_pos_rel_y, float32 p_pos_x, float32 p_pos_y) {
    }
};

}  //namespace licht