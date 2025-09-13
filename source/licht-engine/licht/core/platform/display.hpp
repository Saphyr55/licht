#pragma once

#include "licht/core/platform/display_message_handler.hpp"

#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/core/core_exports.hpp"


namespace licht {


class LICHT_CORE_API Display {
public:
    static constexpr WindowHandle INVALID_WINDOW_HANDLE = UINT32_MAX;
    static constexpr WindowHandle MAIN_WINDOW_HANDLE = 1;
    static constexpr WindowHandle DESKTOP_WINDOW_HANDLE = 0;

public:
    static Display& get_default();

    virtual SharedRef<DisplayMessageHandler> get_message_handler() = 0;

    virtual void set_message_handler(SharedRef<DisplayMessageHandler> message_handler) = 0;

    virtual WindowHandle create_window_handle(const WindowStatues& window_statues) = 0;

    virtual WindowStatues get_window_statues(WindowHandle window) = 0;

    virtual WindowStatues query_window_statues(WindowHandle window) = 0;

    virtual void show(WindowHandle window) = 0;

    virtual void hide(WindowHandle window) = 0;

    virtual void close(WindowHandle window) = 0;

    virtual bool is_valid(WindowHandle window) = 0;

    virtual void handle_events() = 0;

    virtual void* get_native_window_handle(WindowHandle window) = 0;

    virtual ~Display() = default;
};

class LICHT_CORE_API DisplayProvider {
public:
    static Display* get_display();

    static void set_display(Display* display);

private:
    static Display* display_;
};

}  //namespace licht
