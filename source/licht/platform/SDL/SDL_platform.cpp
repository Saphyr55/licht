#include "licht/platform/SDL/SDL_display.hpp"
#include "licht/platform/display.hpp"
#include "licht/platform/platform.hpp"

#include <SDL3/SDL.h>

#include "licht/core/defines.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/core/string/format.hpp"

namespace licht {

void platform_start() {
    
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        LLOG_ERROR("[platform_start]", vformat("Failed to initialize SDL: %s", SDL_GetError()));
        return;
    }

    LLOG_INFO("[platform_start]", "SDL initialized successfully.");

    DisplayProvider::set_display(new SDLDisplay());
}

void platform_end() {
    
    Display* display = DisplayProvider::get_display();

    if (display) {
        LLOG_INFO("[platform_end]", "Cleaning up display resources.");
        delete display;
        DisplayProvider::set_display(nullptr);
    }

    SDL_Quit();
    
    LLOG_INFO("[platform_end]", "SDL cleaned up successfully.");

}

}