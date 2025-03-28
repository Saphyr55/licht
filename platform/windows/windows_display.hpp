#pragma once

#ifdef _WIN32
#include <Windows.h>

#include "core/collection/hash_map.hpp"
#include "core/memory/shared_ref.hpp"
#include "display/display.hpp"

namespace licht {

constexpr const wchar_t g_class_name[] = L"pulsar.engine";

struct WindowsWindowHandle {
    HWND hwnd;
    WNDCLASSEX wc;

    WindowsWindowHandle(HWND in_hwnd, WNDCLASSEX in_wc)
        : hwnd(in_hwnd), wc(in_wc) {}
};

class WindowsDisplay : public Display {
public:
    virtual WindowHandle create_window_handle(const WindowStatues& window_statues) override;

    virtual WindowStatues query_window_statues(WindowHandle window) override;

    virtual SharedRef<DisplayMessageHandler> get_message_handler() override;

    virtual void set_message_handler(SharedRef<DisplayMessageHandler> message_handler) override;

    virtual void show(WindowHandle window) override;

    virtual void hide(WindowHandle window) override;

    virtual void close(WindowHandle window) override;

    virtual bool is_valid(WindowHandle window) override {
        return window != INVALID_WINDOW_HANDLE || windows_.contains(window);
    }

    virtual void pumps_events() override {
        run_message_loop();
    }

    virtual void* get_native_window_handle(WindowHandle window) override;

private:
    void run_message_loop() const;

    friend LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    WindowsDisplay(HINSTANCE hinstance)
        : hinstance_(hinstance)
        , message_handler_(new_ref<DisplayMessageHandler>()) {}

    ~WindowsDisplay() = default;

private:
    SharedRef<DisplayMessageHandler> message_handler_;
    HINSTANCE hinstance_;
    HashMap<HWND, WindowHandle> hwnds_;
    HashMap<WindowHandle, WindowsWindowHandle> windows_;
};

}  //namespace licht

#endif
