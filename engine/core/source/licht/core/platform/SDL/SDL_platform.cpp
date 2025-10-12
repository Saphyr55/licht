#include "licht/core/platform/SDL/SDL_display.hpp"
#include "licht/core/platform/display.hpp"
#include "licht/core/platform/platform.hpp"

#include <SDL3/SDL.h>

#include "licht/core/platform/platform_time.hpp"
#include "licht/core/string/format.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {

void platform_start() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        LLOG_ERROR("[Platform]", vformat("Failed to initialize SDL: %s", SDL_GetError()));
        return;
    }

    platform_time_init();

    LLOG_INFO("[Platform]", "SDL initialized successfully.");
    
    static SDLDisplay g_display = SDLDisplay();

    DisplayProvider::set_display(&g_display);
}

void platform_end() {
    Display* display = DisplayProvider::get_display();

    if (display) {
        LLOG_INFO("[Platform]", "Cleaning up display resources.");
        DisplayProvider::set_display(nullptr);
    }

    SDL_Quit();

    LLOG_INFO("[Platform]", "SDL cleaned up successfully.");
}

}  //namespace licht