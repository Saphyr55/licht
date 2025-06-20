#pragma once

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_keycode.h>

#include "licht/platform/input.hpp"

namespace licht {

Button button_from_SDL(int32 p_button_code);

Key key_from_SDL(SDL_Keycode p_key_code);

int32 button_to_SDL(Button p_button);

SDL_Keycode key_to_SDL(Key p_key);

}