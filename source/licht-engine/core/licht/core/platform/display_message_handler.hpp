#pragma once


#include "licht/core/platform/input.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/core/core_exports.hpp"

namespace licht {

class LICHT_CORE_API DisplayMessageHandler {
public:
    virtual void on_window_close(WindowHandle window) {
    }

    virtual void on_window_resized(WindowHandle window, uint32 width, uint32 height) {
    }

    virtual void on_window_minimized(WindowHandle window) {
    }

    virtual void on_window_shown(WindowHandle window) {
    }

    virtual void on_mouse_wheel(float32 delta) {
    }

    virtual void on_key_up(Key key) {
    }

    virtual void on_key_down(Key key) {
    }

    virtual void on_button_up(Button button) {
    }

    virtual void on_button_down(Button button) {
    }

    virtual void on_mouse_move(float32 pos_rel_x, float32 pos_rel_y, float32 pos_x, float32 pos_y) {
    }

    virtual ~DisplayMessageHandler() = default;
};

}  //namespace licht