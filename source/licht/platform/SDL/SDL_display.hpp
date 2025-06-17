#pragma once

#include <SDL3/SDL.h>

#include "licht/core/collection/hash_map.hpp"
#include "licht/platform/display.hpp"
#include "licht/platform/window_handle.hpp"


namespace licht {

class SDLDisplay : public Display {
public:
    virtual SharedRef<DisplayMessageHandler> get_message_handler() override;

    virtual void set_message_handler(SharedRef<DisplayMessageHandler> message_handler) override;

    virtual WindowHandle create_window_handle(const WindowStatues& window_statues) override;

    virtual WindowStatues get_window_statues(WindowHandle window) override;

    virtual WindowStatues query_window_statues(WindowHandle window) override;

    virtual void show(WindowHandle window) override;

    virtual void hide(WindowHandle window) override;

    virtual void close(WindowHandle window) override;

    virtual bool is_valid(WindowHandle window) override;

    virtual void handle_events() override;

    virtual void* get_native_window_handle(WindowHandle window) override;

private:
    SharedRef<DisplayMessageHandler> message_handler_;
    HashMap<WindowHandle, SDL_Window*> main_window_map_;
    HashMap<WindowHandle, WindowStatues> main_window_statues_map_;
};

}