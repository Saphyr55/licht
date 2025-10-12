#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/platform/display_message_handler.hpp"
#include "licht/core/platform/input.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/core/signals/signal.hpp"
#include "render_frame_script.hpp"

using namespace licht;

class DemoMessageHandler final : public DisplayMessageHandler {
public:
    virtual void on_window_close(WindowHandle window) override;

    virtual void on_window_resized(WindowHandle window, uint32 width, uint32 height) override;

    virtual void on_window_minimized(WindowHandle window) override;

    virtual void on_window_shown(WindowHandle window) override;

    virtual void on_mouse_wheel(float32 delta) override;

    virtual void on_key_down(VirtualKey key) override;

    virtual void on_key_up(VirtualKey key) override;
    
    virtual void on_mouse_move(float32 pos_rel_x, float32 pos_rel_y, float32 pos_x, float32 pos_y) override;

    virtual void on_button_up(Button button) override;

    virtual void on_button_down(Button button) override;

    inline void set_render_frame_script(RenderFrameScript* frame_script) {
        frame_script_ = frame_script;
    }

private:
    RenderFrameScript* frame_script_ = nullptr;
};

inline bool g_is_engine_running = false;
inline bool g_is_app_running = false;
