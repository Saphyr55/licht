#include "SDL_display.hpp"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include "licht/core/defines.hpp"
#include "licht/core/string/format.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/platform/SDL/SDL_input.hpp"

namespace licht {

void SDLDisplay::set_message_handler(SharedRef<DisplayMessageHandler> p_message_handler) {
    message_handler_ = p_message_handler;
}

SharedRef<DisplayMessageHandler> SDLDisplay::get_message_handler() {
    return message_handler_;
}

WindowHandle SDLDisplay::create_window_handle(const WindowStatues& p_window_statues){ 
    
    SDL_Window* sdl_window = SDL_CreateWindow(
        p_window_statues.title.data(),
        p_window_statues.width,
        p_window_statues.height,
          SDL_WINDOW_RESIZABLE 
        | SDL_WINDOW_HIDDEN 
        | SDL_WINDOW_VULKAN
    );

    if (!sdl_window) {
        LLOG_ERROR("[SDLDisplay::create_window_handle]", vformat("Failed to create window: %s", SDL_GetError()));
        return Display::INVALID_WINDOW_HANDLE;
    }

    WindowHandle handle = main_window_map_.size() + 1;
    main_window_map_[handle] = sdl_window;
    main_window_statues_map_[handle] = p_window_statues;

    return handle;
}

WindowStatues SDLDisplay::get_window_statues(WindowHandle p_window) {

    LCHECK_MSG(is_valid(p_window), "Invalid window handle provided.");

    WindowStatues statues = main_window_statues_map_[p_window];
    
    LLOG_INFO("[SDLDisplay::get_window_statues]", vformat("Retrieved window statues for handle: %d", p_window));
    
    return statues;
}

WindowStatues SDLDisplay::query_window_statues(WindowHandle p_window) { 

    LCHECK_MSG(is_valid(p_window), "Invalid window handle provided.");
    
    SDL_Window* sdl_window = main_window_map_[p_window];
    
    LCHECK(sdl_window);

    WindowStatues& statues = main_window_statues_map_[p_window];

    int32 width, height;
    SDL_GetWindowSize(sdl_window, &width, &height);
    statues.width = static_cast<float32>(width);
    statues.height = static_cast<float32>(height);

    int32 x, y;
    SDL_GetWindowPosition(sdl_window, &x, &y);
    statues.x = static_cast<float32>(x);
    statues.y = static_cast<float32>(y);

    statues.title = SDL_GetWindowTitle(sdl_window);
    
    LLOG_INFO("[SDLDisplay::query_window_statues]", vformat("Queried window statues for handle: %d", p_window));
    
    return statues;
}

void SDLDisplay::show(WindowHandle p_window) {

    LCHECK_MSG(is_valid(p_window), "Invalid window handle provided.");
    
    SDL_Window* sdl_window = main_window_map_[p_window];
    
    LCHECK(sdl_window);
    
    SDL_ShowWindow(sdl_window);
    
    LLOG_INFO("[SDLDisplay::show]", vformat("Showing window with handle: %d", p_window));
}

void SDLDisplay::hide(WindowHandle p_window) {

    LCHECK_MSG(is_valid(p_window), "Invalid window handle provided.");

    SDL_Window* sdl_window = main_window_map_[p_window];

    LCHECK(sdl_window);

    SDL_HideWindow(sdl_window);    
}

void SDLDisplay::close(WindowHandle p_window) {
    LCHECK_MSG(is_valid(p_window), "Invalid window handle provided.");
    
    SDL_Window* sdl_window = main_window_map_[p_window];

    LCHECK(sdl_window);
    
    SDL_DestroyWindow(sdl_window);
}

bool SDLDisplay::is_valid(WindowHandle window) {
    if (window == Display::INVALID_WINDOW_HANDLE) {
        return false;
    }

    return main_window_map_.contains(window) && main_window_map_[window] != nullptr;
}

void SDLDisplay::handle_events() { 

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
                WindowHandle window = event.window.windowID;
                if (message_handler_) {
                    message_handler_->on_window_close(window);
                }
                break;
            }
            case SDL_EVENT_WINDOW_RESIZED: {
                WindowHandle window = event.window.windowID;
                if (message_handler_) {
                    message_handler_->on_window_resized(window, event.window.data1, event.window.data2);
                }
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL: {
                if (message_handler_) {
                    message_handler_->on_mouse_wheel(event.wheel.mouse_x);
                }
                break;
            }
            case SDL_EVENT_KEY_UP: {
                if (message_handler_) {
                    message_handler_->on_key_up(key_from_SDL(event.key.key));
                }
                break;
            }
            case SDL_EVENT_KEY_DOWN: {
                if (message_handler_) {
                    message_handler_->on_key_down(key_from_SDL(event.key.key));
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                if (message_handler_) {
                    message_handler_->on_button_up(button_from_SDL(event.button.button));
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                if (message_handler_) {
                    message_handler_->on_button_down(button_from_SDL(event.button.button));
                }
                break;
            }
            case SDL_EVENT_MOUSE_MOTION: {
                if (message_handler_) {
                    message_handler_->on_mouse_move(event.motion.xrel, event.motion.yrel, event.motion.x, event.motion.y);
                }
                break;
            }
        }
    }
}

void* SDLDisplay::get_native_window_handle(WindowHandle window) {
    LCHECK_MSG(is_valid(window), "Invalid window handle provided.");
    
    SDL_Window* sdl_window = main_window_map_[window];
    
    LCHECK(sdl_window);

    return static_cast<void*>(sdl_window);
}

}