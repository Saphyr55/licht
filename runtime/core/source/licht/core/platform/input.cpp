#include "licht/core/platform/input.hpp"

namespace licht {

Signal<const VirtualKey&> Input::on_key_down;
Signal<const VirtualKey&> Input::on_key_up;
Signal<const VirtualKey&> Input::on_key_pressed;
Signal<const VirtualKey&> Input::on_key_release;

Signal<const MouseMove&> Input::on_mouse_move;
Signal<const MouseWheel&> Input::on_mouse_wheel;

Signal<const Button&> Input::on_button_down;
Signal<const Button&> Input::on_button_up;
Signal<const Button&> Input::on_button_pressed;
Signal<const Button&> Input::on_button_release;

HashSet<VirtualKey> Input::keys_dow_;
HashSet<VirtualKey> Input::keys_up_;
HashSet<VirtualKey> Input::keys_pressed_;
HashSet<VirtualKey> Input::keys_release_;

HashSet<Button> Input::buttons_down_;
HashSet<Button> Input::buttons_up_;
HashSet<Button> Input::buttons_pressed_;
HashSet<Button> Input::buttons_release_;

bool Input::key_is_down(VirtualKey key) {
    return keys_dow_.contains(key);
}

bool Input::key_is_up(VirtualKey key) {
    return keys_up_.contains(key);
}

bool Input::key_is_pressed(VirtualKey key) {
    return keys_pressed_.contains(key);
}

bool Input::key_is_release(VirtualKey key) {
    return keys_release_.contains(key);
}

bool Input::button_is_down(Button button) {
    return buttons_down_.contains(button);
}

bool Input::button_is_up(Button button) {
    return buttons_up_.contains(button);
}

bool Input::button_is_pressed(Button button) {
    return buttons_pressed_.contains(button);
}

bool Input::button_is_release(Button button) {
    return buttons_release_.contains(button);
}

StringRef key_to_string(VirtualKey key) {
    switch (key) {
        case VirtualKey::A: return "A";
        case VirtualKey::B: return "B";
        case VirtualKey::C: return "C";
        case VirtualKey::D: return "D";
        case VirtualKey::E: return "E";
        case VirtualKey::F: return "F";
        case VirtualKey::G: return "G";
        case VirtualKey::H: return "H";
        case VirtualKey::I: return "I";
        case VirtualKey::J: return "J";
        case VirtualKey::K: return "K";
        case VirtualKey::L: return "L";
        case VirtualKey::M: return "M";
        case VirtualKey::N: return "N";
        case VirtualKey::O: return "O";
        case VirtualKey::P: return "P";
        case VirtualKey::Q: return "Q";
        case VirtualKey::R: return "R";
        case VirtualKey::S: return "S";
        case VirtualKey::T: return "T";
        case VirtualKey::U: return "U";
        case VirtualKey::V: return "V";
        case VirtualKey::W: return "W";
        case VirtualKey::X: return "X";
        case VirtualKey::Y: return "Y";
        case VirtualKey::Z: return "Z";

        case VirtualKey::Zero: return "0";
        case VirtualKey::One: return "1";
        case VirtualKey::Two: return "2";
        case VirtualKey::Three: return "3";
        case VirtualKey::Four: return "4";
        case VirtualKey::Five: return "5";
        case VirtualKey::Six: return "6";
        case VirtualKey::Seven: return "7";
        case VirtualKey::Eight: return "8";
        case VirtualKey::Nine: return "9";

        case VirtualKey::F1: return "F1";
        case VirtualKey::F2: return "F2";
        case VirtualKey::F3: return "F3";
        case VirtualKey::F4: return "F4";
        case VirtualKey::F5: return "F5";
        case VirtualKey::F6: return "F6";
        case VirtualKey::F7: return "F7";
        case VirtualKey::F8: return "F8";
        case VirtualKey::F9: return "F9";
        case VirtualKey::F10: return "F10";
        case VirtualKey::F11: return "F11";
        case VirtualKey::F12: return "F12";


        case VirtualKey::Left: return "Left";
        case VirtualKey::Right: return "Right";
        case VirtualKey::Up: return "Up";
        case VirtualKey::Down: return "Down";
        case VirtualKey::Return: return "Enter";
        case VirtualKey::Escape:
            return "Escape";
        case VirtualKey::Space: return "Space";
        case VirtualKey::Tab: return "Tab";
        case VirtualKey::Back: return "Backspace";
        case VirtualKey::CapsLock: return "Caps Lock";

        case VirtualKey::Period: return ".";
        case VirtualKey::Comma: return ",";
        case VirtualKey::Minus: return "-";
        case VirtualKey::Plus: return "+";

        case VirtualKey::Insert: return "Insert";
        case VirtualKey::Delete: return "Delete";
        case VirtualKey::Home: return "Home";
        case VirtualKey::End: return "End";
        case VirtualKey::PageUp: return "Page Up";
        case VirtualKey::PageDown: return "Page Down";

        case VirtualKey::LeftShift: return "Left Shift";
        case VirtualKey::RightShift: return "Right Shift";
        case VirtualKey::LeftControl: return "Left Ctrl";
        case VirtualKey::RightControl: return "Right Ctrl";
        case VirtualKey::LeftAlt: return "Left Alt";
        case VirtualKey::RightAlt: return "Right Alt";

        case VirtualKey::KP_Plus: return "Numpad +";
        case VirtualKey::KP_Minus: return "Numpad -";
        case VirtualKey::KP_Multiply: return "Numpad *";
        case VirtualKey::KP_Divide: return "Numpad /";
        case VirtualKey::KP_Enter: return "Numpad Enter";
        case VirtualKey::KP_0: return "Numpad 0";
        case VirtualKey::KP_1: return "Numpad 1";
        case VirtualKey::KP_2: return "Numpad 2";
        case VirtualKey::KP_3: return "Numpad 3";
        case VirtualKey::KP_4: return "Numpad 4";
        case VirtualKey::KP_5: return "Numpad 5";
        case VirtualKey::KP_6: return "Numpad 6";
        case VirtualKey::KP_7: return "Numpad 7";
        case VirtualKey::KP_8: return "Numpad 8";
        case VirtualKey::KP_9: return "Numpad 9";
        // case Key::KP_PERIOD: return "Numpad .";

        case VirtualKey::Mute: return "Mute";
        case VirtualKey::VolumeDown: return "Volume Down";
        case VirtualKey::VolumeUp: return "Volume Up";
        case VirtualKey::KApp: return "Application";

        case VirtualKey::None:
        default: return "Unknown";
    }
}

}