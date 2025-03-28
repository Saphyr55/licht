#ifdef _WIN32

#include "platform/windows/windows_display.hpp"
#include "core/defines.hpp"
#include "core/memory/shared_ref.hpp"
#include "display/display.hpp"
#include "display/display_message_handler.hpp"
#include "display/display_input.hpp"

#include <Windows.h>
#include <Windowsx.h>


namespace licht {

static double g_clock_frequency;

/** */
static LARGE_INTEGER g_start_time;

/** */
static float g_last_mouse_x = 0;

/** */
static float g_last_mouse_y = 0;


static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void WindowsDisplay::run_message_loop() const {
    MSG msg = {};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void* WindowsDisplay::get_native_window_handle(WindowHandle window) {
    LCHECK_MSG(is_valid(window), "Invalid window handle.");
    return static_cast<void*>(windows_[window].hwnd);
}

WindowHandle WindowsDisplay::create_window_handle(const WindowStatues& window_statues) {
    static WindowHandle last_window_handle = MAIN_WINDOW_HANDLE;

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hinstance_;
    wc.lpszClassName = g_class_name;

    if (!RegisterClassEx(&wc)) {
        return false;
    }

    HWND hwnd = CreateWindowEx(
        0,                                                // Optional window styles.
        g_class_name,                                     // Window class.
        String::unicode_of(window_statues.title.data()),  // Window text.
        WS_OVERLAPPEDWINDOW,                              // Window style.
        // Position.
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        // Size.
        window_statues.width,
        window_statues.height,
        NULL,        // Parent window.
        NULL,        // Menu.
        hinstance_,  // Instance handle.
        NULL         // Additional application data.
    );

    if (hwnd == NULL) {
        return INVALID_WINDOW_HANDLE;
    }

    WindowHandle window = windows_.insert(
        last_window_handle++, 
        WindowsWindowHandle(hwnd, wc)).key;
    
    hwnds_[hwnd] = window;

    return window;
}

WindowStatues WindowsDisplay::query_window_statues(WindowHandle window) {
    LCHECK_MSG(is_valid(window), "Invalid window handle.");

    RECT rect;
    GetWindowRect(windows_[window].hwnd, &rect);

    WindowStatues window_statues;
    window_statues.width = rect.right - rect.left;
    window_statues.height = rect.bottom - rect.top;
    window_statues.x = rect.left;
    window_statues.y = rect.top;

    return window_statues;
}

SharedRef<DisplayMessageHandler> WindowsDisplay::get_message_handler() {
    return message_handler_;
}

void WindowsDisplay::set_message_handler(SharedRef<DisplayMessageHandler> message_handler) {
    message_handler_ = message_handler;
}

void WindowsDisplay::show(WindowHandle window) {
    LCHECK_MSG(is_valid(window), "Invalid window handle.");
    ShowWindow(windows_[window].hwnd, SW_SHOW);
}

void WindowsDisplay::hide(WindowHandle window) {
    LCHECK_MSG(is_valid(window), "Invalid window handle.");
    ShowWindow(windows_[window].hwnd, SW_HIDE);
}

void WindowsDisplay::close(WindowHandle window) {
    LCHECK_MSG(is_valid(window), "Invalid window handle.");
    hwnds_.remove(windows_[window].hwnd);
    DestroyWindow(windows_[window].hwnd);
    windows_.remove(window);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    WindowsDisplay& display = reinterpret_cast<WindowsDisplay&>(Display::get_default());

    SharedRef<DisplayMessageHandler> handler = Display::get_default().get_message_handler();
    WindowHandle window = display.hwnds_[hwnd];

    LCHECK(handler)

    switch (msg) {
        case WM_SIZE: {
            if (window == Display::INVALID_WINDOW_HANDLE || !handler) {
                break;
            }
            uint32 width = LOWORD(lParam);
            uint32 height = HIWORD(lParam);
            handler->on_window_resize(window, width, height);
            break;
        }
        case WM_SYSKEYUP:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN: {
            if (window == Display::INVALID_WINDOW_HANDLE || !handler) {
                break;
            }
            Key key = static_cast<Key>(wParam);

            bool down = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            bool repeat = (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT;

            if (down) {
                handler->on_key_down(key);
            } else {
                handler->on_key_up(key);
            }

            break;
        }
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN: {
            if (window == Display::INVALID_WINDOW_HANDLE || !handler) {
                break;
            }
            Button button;
            bool down =
                msg == WM_LBUTTONDOWN ||
                msg == WM_RBUTTONDOWN ||
                msg == WM_MBUTTONDOWN;

            switch (msg) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    button = Button::Left;
                    break;
                case WM_MBUTTONUP:
                case WM_MBUTTONDOWN:
                    button = Button::Middle;
                    break;
                case WM_RBUTTONUP:
                case WM_RBUTTONDOWN:
                    button = Button::Right;
                    break;
                default:
                    break;
            }

            if (down) {
                handler->on_button_down(button);
            } else {
                handler->on_button_up(button);
            }

            break;
        }
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP: {
            if (window == Display::INVALID_WINDOW_HANDLE || !handler) {
                break;
            }
            Button button = GET_XBUTTON_WPARAM(wParam) == XBUTTON1
                                       ? Button::XButton1
                                       : Button::XButton2;

            if (msg == WM_XBUTTONDOWN) {
                handler->on_button_down(button);
            } else {
                handler->on_button_up(button);
            }
            break;
        }
        case WM_NCMOUSEMOVE:
        case WM_MOUSEMOVE: {
            if (window == Display::INVALID_WINDOW_HANDLE || !handler) {
                break;
            }
            float32 x = GET_X_LPARAM(lParam);
            float32 y = GET_Y_LPARAM(lParam);
            float32 rel_x = x - g_last_mouse_x;
            float32 rel_y = y - g_last_mouse_y;
            g_last_mouse_x = x;
            g_last_mouse_y = y;
            handler->on_mouse_move(rel_x, rel_y, x, y);
            break;
        }
        case WM_MOUSEWHEEL: {
            if (window == Display::INVALID_WINDOW_HANDLE || !handler) {
                break;
            }
            float delta = GET_WHEEL_DELTA_WPARAM(wParam);
            handler->on_mouse_wheel(delta);
            break;
        }
        case WM_CLOSE: {
            if (window == Display::INVALID_WINDOW_HANDLE || !handler) {
                return 0;
            }
            handler->on_window_close(window);
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        default:
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

}  //namespace licht

#endif // _WIN32
