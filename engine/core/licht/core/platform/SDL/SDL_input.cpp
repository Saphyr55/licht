#include "licht/core/platform/SDL/SDL_input.hpp"

namespace licht {

Button button_from_SDL(int32 sdl_button) {
    switch (sdl_button) {
        case SDL_BUTTON_LEFT:   return Button::Left;
        case SDL_BUTTON_MIDDLE: return Button::Middle;
        case SDL_BUTTON_RIGHT:  return Button::Right;
        case SDL_BUTTON_X1:     return Button::XButton1;
        case SDL_BUTTON_X2:     return Button::XButton2;
        default:                return Button::Left;
    }
}

Key key_from_SDL(SDL_Keycode p_key_code) {
     switch (p_key_code) {
        case SDLK_A: return Key::A;
        case SDLK_B: return Key::B;
        case SDLK_C: return Key::C;
        case SDLK_D: return Key::D;
        case SDLK_E: return Key::E;
        case SDLK_F: return Key::F;
        case SDLK_G: return Key::G;
        case SDLK_H: return Key::H;
        case SDLK_I: return Key::I;
        case SDLK_J: return Key::J;
        case SDLK_K: return Key::K;
        case SDLK_L: return Key::L;
        case SDLK_M: return Key::M;
        case SDLK_N: return Key::N;
        case SDLK_O: return Key::O;
        case SDLK_P: return Key::P;
        case SDLK_Q: return Key::Q;
        case SDLK_R: return Key::R;
        case SDLK_S: return Key::S;
        case SDLK_T: return Key::T;
        case SDLK_U: return Key::U;
        case SDLK_V: return Key::V;
        case SDLK_W: return Key::W;
        case SDLK_X: return Key::X;
        case SDLK_Y: return Key::Y;
        case SDLK_Z: return Key::Z;

        case SDLK_0: return Key::Zero;
        case SDLK_1: return Key::One;
        case SDLK_2: return Key::Two;
        case SDLK_3: return Key::Three;
        case SDLK_4: return Key::Four;
        case SDLK_5: return Key::Five;
        case SDLK_6: return Key::Six;
        case SDLK_7: return Key::Seven;
        case SDLK_8: return Key::Eight;
        case SDLK_9: return Key::Nine;

        case SDLK_F1: return Key::F1;
        case SDLK_F2: return Key::F2;
        case SDLK_F3: return Key::F3;
        case SDLK_F4: return Key::F4;
        case SDLK_F5: return Key::F5;
        case SDLK_F6: return Key::F6;
        case SDLK_F7: return Key::F7;
        case SDLK_F8: return Key::F8;
        case SDLK_F9: return Key::F9;
        case SDLK_F10: return Key::F10;
        case SDLK_F11: return Key::F11;
        case SDLK_F12: return Key::F12;

        case SDLK_LEFT: return Key::Left;
        case SDLK_RIGHT: return Key::Right;
        case SDLK_UP: return Key::Up;
        case SDLK_DOWN: return Key::Down;
        case SDLK_SPACE: return Key::Space;
        case SDLK_RETURN: return Key::Return;
        case SDLK_TAB: return Key::Tab;
        case SDLK_BACKSPACE: return Key::Back;
        case SDLK_MINUS: return Key::Minus;
        case SDLK_EQUALS: return Key::Plus;
        case SDLK_COMMA: return Key::Comma;
        case SDLK_PERIOD: return Key::Period;
        case SDLK_INSERT: return Key::Insert;
        case SDLK_DELETE: return Key::Delete;
        case SDLK_HOME: return Key::Home;
        case SDLK_END: return Key::End;
        case SDLK_PAGEUP: return Key::PageUp;
        case SDLK_PAGEDOWN: return Key::PageDown;

        case SDLK_LSHIFT: return Key::LeftShift;
        case SDLK_RSHIFT: return Key::RightShift;
        case SDLK_LCTRL: return Key::LeftControl;
        case SDLK_RCTRL: return Key::RightControl;
        case SDLK_LALT: return Key::LeftAlt;
        case SDLK_RALT: return Key::RightAlt;
        case SDLK_CAPSLOCK: return Key::CapsLock;

        case SDLK_MUTE: return Key::Mute;
        case SDLK_VOLUMEDOWN: return Key::VolumeDown;
        case SDLK_VOLUMEUP: return Key::VolumeUp;

        case SDLK_KP_PLUS: return Key::KP_PLUS;
        case SDLK_KP_MINUS: return Key::KP_MINUS;
        case SDLK_KP_MULTIPLY: return Key::KP_MULTIPLY;
        case SDLK_KP_DIVIDE: return Key::KP_DIVIDE;
        case SDLK_KP_ENTER: return Key::KP_ENTER;

        case SDLK_KP_1: return Key::KP_1;
        case SDLK_KP_2: return Key::KP_2;
        case SDLK_KP_3: return Key::KP_3;
        case SDLK_KP_4: return Key::KP_4;
        case SDLK_KP_5: return Key::KP_5;
        case SDLK_KP_6: return Key::KP_6;
        case SDLK_KP_7: return Key::KP_7;
        case SDLK_KP_8: return Key::KP_8;
        case SDLK_KP_9: return Key::KP_9;

        case SDLK_KP_0: return Key::KP_0;
        case SDLK_KP_DECIMAL: return Key::KP_PERIOD;

        default: return Key::None;
    }
}

int32 button_to_SDL(Button b) {
    switch (b) {
        case Button::Left:     return SDL_BUTTON_LEFT;   // = 1
        case Button::Middle:   return SDL_BUTTON_MIDDLE; // = 2
        case Button::Right:    return SDL_BUTTON_RIGHT;  // = 3
        case Button::XButton1: return SDL_BUTTON_X1;     // = 4
        case Button::XButton2: return SDL_BUTTON_X2;     // = 5
        default: {
            return 0;
        }
    }
}

SDL_Keycode key_to_SDL(Key k) {
    switch (k) {
        case Key::A: return SDLK_A;
        case Key::B: return SDLK_B;
        case Key::C: return SDLK_C;
        case Key::D: return SDLK_D;
        case Key::E: return SDLK_E;
        case Key::F: return SDLK_F;
        case Key::G: return SDLK_G;
        case Key::H: return SDLK_H;
        case Key::I: return SDLK_I;
        case Key::J: return SDLK_J;
        case Key::K: return SDLK_K;
        case Key::L: return SDLK_L;
        case Key::M: return SDLK_M;
        case Key::N: return SDLK_N;
        case Key::O: return SDLK_O;
        case Key::P: return SDLK_P;
        case Key::Q: return SDLK_Q;
        case Key::R: return SDLK_R;
        case Key::S: return SDLK_S;
        case Key::T: return SDLK_T;
        case Key::U: return SDLK_U;
        case Key::V: return SDLK_V;
        case Key::W: return SDLK_W;
        case Key::X: return SDLK_X;
        case Key::Y: return SDLK_Y;
        case Key::Z: return SDLK_Z;

        case Key::Zero: return SDLK_0;
        case Key::One:  return SDLK_1;
        case Key::Two:  return SDLK_2;
        case Key::Three:return SDLK_3;
        case Key::Four: return SDLK_4;
        case Key::Five: return SDLK_5;
        case Key::Six:  return SDLK_6;
        case Key::Seven:return SDLK_7;
        case Key::Eight:return SDLK_8;
        case Key::Nine: return SDLK_9;

        case Key::F1: return SDLK_F1;
        case Key::F2:  return SDLK_F2;
        case Key::F3:  return SDLK_F3;
        case Key::F4:  return SDLK_F4;
        case Key::F5:  return SDLK_F5;
        case Key::F6:  return SDLK_F6;
        case Key::F7:  return SDLK_F7;
        case Key::F8:  return SDLK_F8;
        case Key::F9:  return SDLK_F9;
        case Key::F10: return SDLK_F10;
        case Key::F11: return SDLK_F11;
        case Key::F12: return SDLK_F12;

        case Key::Left: return SDLK_LEFT;
        case Key::Right: return SDLK_RIGHT;
        case Key::Up: return SDLK_UP;
        case Key::Down: return SDLK_DOWN;
        case Key::Space: return SDLK_SPACE;
        case Key::Return: return SDLK_RETURN;
        case Key::Tab: return SDLK_TAB;
        case Key::Back: return SDLK_BACKSPACE;
        case Key::Minus: return SDLK_MINUS;
        case Key::Plus: return SDLK_PLUS;
        case Key::Comma: return SDLK_COMMA;
        case Key::Period: return SDLK_PERIOD;
        case Key::Insert: return SDLK_INSERT;
        case Key::Delete: return SDLK_DELETE;
        case Key::Home: return SDLK_HOME;
        case Key::End: return SDLK_END;
        case Key::PageUp: return SDLK_PAGEUP;
        case Key::PageDown: return SDLK_PAGEDOWN;

        case Key::LeftShift: return SDLK_LSHIFT;
        case Key::RightShift: return SDLK_RSHIFT;
        case Key::LeftControl: return SDLK_LCTRL;
        case Key::RightControl: return SDLK_RCTRL;
        case Key::LeftAlt: return SDLK_LALT;
        case Key::RightAlt: return SDLK_RALT;
        case Key::CapsLock: return SDLK_CAPSLOCK;
        case Key::Mute: return SDLK_MUTE;
        case Key::VolumeDown: return SDLK_VOLUMEDOWN;
        case Key::VolumeUp: return SDLK_VOLUMEUP;
        case Key::KP_PLUS: return SDLK_KP_PLUS;
        case Key::KP_MINUS: return SDLK_KP_MINUS;
        case Key::KP_MULTIPLY: return SDLK_KP_MULTIPLY;
        case Key::KP_DIVIDE: return SDLK_KP_DIVIDE;
        case Key::KP_ENTER: return SDLK_KP_ENTER;
        
        case Key::KP_1: return SDLK_KP_1;
        case Key::KP_2: return SDLK_KP_2;
        case Key::KP_3: return SDLK_KP_3;
        case Key::KP_4: return SDLK_KP_4;
        case Key::KP_5: return SDLK_KP_5;
        case Key::KP_6: return SDLK_KP_6;
        case Key::KP_7: return SDLK_KP_7;
        case Key::KP_8: return SDLK_KP_8;
        case Key::KP_9: return SDLK_KP_9;
        
        // case Key::KP_PERIOD: return SDLK_KP_DECIMAL;

        default: return SDLK_UNKNOWN;
    }
}

}
