#pragma once

#include "licht/core/platform/display_message_handler.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/rhi/rhi_module.hpp"

using namespace licht;

class DemoMessageHandler final : public DisplayMessageHandler {
public:
    virtual void on_window_close(WindowHandle window) override;

    virtual void on_window_resized(WindowHandle window, uint32 width, uint32 height) override;

    virtual void on_window_minimized(WindowHandle window) override;

    virtual void on_window_shown(WindowHandle window) override;

    virtual void on_mouse_wheel(float32 delta) override;

    virtual void on_key_down(Key key) override;

    inline void set_rhi_module(RHIModule* rhi_module) {
        rhi_module_ = rhi_module;
    }

private:
    RHIModule* rhi_module_ = nullptr;
};

inline bool g_is_running = false;
