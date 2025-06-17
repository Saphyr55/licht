#include "licht/platform/input.hpp"


namespace licht {

StringRef key_to_string(Key p_key) {
    switch (p_key) {
        case Key::A: return "A";
        case Key::B: return "B";
        case Key::C: return "C";
        case Key::D: return "D";
        case Key::E: return "E";
        case Key::F: return "F";
        case Key::G: return "G";
        case Key::H: return "H";
        case Key::I: return "I";
        case Key::J: return "J";
        case Key::K: return "K";
        case Key::L: return "L";
        case Key::M: return "M";
        case Key::N: return "N";
        case Key::O: return "O";
        case Key::P: return "P";
        case Key::Q: return "Q";
        case Key::R: return "R";
        case Key::S: return "S";
        case Key::T: return "T";
        case Key::U: return "U";
        case Key::V: return "V";
        case Key::W: return "W";
        case Key::X: return "X";
        case Key::Y: return "Y";
        case Key::Z: return "Z";

        case Key::Zero: return "0";
        case Key::One: return "1";
        case Key::Two: return "2";
        case Key::Three: return "3";
        case Key::Four: return "4";
        case Key::Five: return "5";
        case Key::Six: return "6";
        case Key::Seven: return "7";
        case Key::Eight: return "8";
        case Key::Nine: return "9";

        case Key::F1: return "F1";
        case Key::F2: return "F2";
        case Key::F3: return "F3";
        case Key::F4: return "F4";
        case Key::F5: return "F5";
        case Key::F6: return "F6";
        case Key::F7: return "F7";
        case Key::F8: return "F8";
        case Key::F9: return "F9";
        case Key::F10: return "F10";
        case Key::F11: return "F11";
        case Key::F12: return "F12";

        case Key::Left: return "Left";
        case Key::Right: return "Right";
        case Key::Up: return "Up";
        case Key::Down: return "Down";
        case Key::Return: return "Enter";
        case Key::Space: return "Space";
        case Key::Tab: return "Tab";
        case Key::Back: return "Backspace";
        case Key::CapsLock: return "Caps Lock";

        case Key::Period: return ".";
        case Key::Comma: return ",";
        case Key::Minus: return "-";
        case Key::Plus: return "+";

        case Key::Insert: return "Insert";
        case Key::Delete: return "Delete";
        case Key::Home: return "Home";
        case Key::End: return "End";
        case Key::PageUp: return "Page Up";
        case Key::PageDown: return "Page Down";

        case Key::LeftShift: return "Left Shift";
        case Key::RightShift: return "Right Shift";
        case Key::LeftControl: return "Left Ctrl";
        case Key::RightControl: return "Right Ctrl";
        case Key::LeftAlt: return "Left Alt";
        case Key::RightAlt: return "Right Alt";

        case Key::KP_PLUS: return "Numpad +";
        case Key::KP_MINUS: return "Numpad -";
        case Key::KP_MULTIPLY: return "Numpad *";
        case Key::KP_DIVIDE: return "Numpad /";
        case Key::KP_ENTER: return "Numpad Enter";
        case Key::KP_0: return "Numpad 0";
        case Key::KP_1: return "Numpad 1";
        case Key::KP_2: return "Numpad 2";
        case Key::KP_3: return "Numpad 3";
        case Key::KP_4: return "Numpad 4";
        case Key::KP_5: return "Numpad 5";
        case Key::KP_6: return "Numpad 6";
        case Key::KP_7: return "Numpad 7";
        case Key::KP_8: return "Numpad 8";
        case Key::KP_9: return "Numpad 9";
        // case Key::KP_PERIOD: return "Numpad .";

        case Key::Mute: return "Mute";
        case Key::VolumeDown: return "Volume Down";
        case Key::VolumeUp: return "Volume Up";
        case Key::KApp: return "Application";

        case Key::None:
        default: return "Unknown";
    }
}

}