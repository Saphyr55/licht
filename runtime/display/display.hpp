#pragma once

#include "display/display_exports.hpp"
#include "core/string/string_ref.hpp"
#include "core/memory/shared_ref.hpp"
#include "core/defines.hpp"
#include "display/display_message_handler.hpp"

namespace licht {

using WindowHandle = uint32;

struct LICHT_DISPLAY_API WindowStatues {
    StringRef title;

    float width;
    float height;

    float x;
    float y;
};

class LICHT_DISPLAY_API Display {
public:
    static constexpr WindowHandle INVALID_WINDOW_HANDLE = UINT32_MAX;

    static constexpr WindowHandle MAIN_WINDOW_HANDLE = 0;

public:
    static Display& get_default();

    virtual SharedRef<DisplayMessageHandler> get_message_handler() = 0;

    virtual void set_message_handler(SharedRef<DisplayMessageHandler> message_handler) = 0;

    virtual WindowHandle create_window_handle(const WindowStatues& window_statues) = 0;

    virtual WindowStatues query_window_statues(WindowHandle window) = 0;

    virtual void show(WindowHandle window) = 0;

    virtual void hide(WindowHandle window) = 0;

    virtual void close(WindowHandle window) = 0;

    virtual bool is_valid(WindowHandle window) = 0;

    virtual void pumps_events() = 0;

    virtual void* get_native_window_handle(WindowHandle window) = 0;

    virtual ~Display() = default;
};

class LICHT_DISPLAY_API DisplayProvider {
public:
    static Display* get_display();

    static void set_display(Display* display);

private:
    static Display* display_;
};

}  //namespace licht
