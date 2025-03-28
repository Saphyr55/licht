#pragma once

#include "display/display_exports.hpp"
#include "display/display.hpp"
#include "display/display_input.hpp"

namespace licht {

class LICHT_DISPLAY_API DisplayMessageHandler {
public:
    virtual void on_window_close(uint32 window) {
    }

    virtual void on_window_resize(uint32 window, uint32 width, uint32 height) {
    }

    virtual void on_mouse_wheel(float delta) {
    }

    virtual void on_key_up(Key key) {
    }

    virtual void on_key_down(Key key) {
    }

    virtual void on_button_up(Button button) {
    }

    virtual void on_button_down(Button button) {
    }

    virtual void on_mouse_move(float posRelX, float posRelY, float posX, float posY) {
    }
};

}  //namespace licht