#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/signals/signal.hpp"
#include "licht/core/string/string_ref.hpp"

#include <set>

namespace licht {

struct MouseMove {
    float32 pos_rel_x;
    float32 pos_rel_y;
    float32 pos_x;
    float32 pos_y;
};

struct MouseWheel {
    float32 delta;
};

enum class Button : uint8 {
    Left = 1,
    Middle = 2,
    Right = 3,
    XButton1 = 4,
    XButton2 = 5
};

enum class VirtualKey : uint32 {
    None,

    Escape = 0x07,
    Back = 0x08,
    Tab = 0x09,
    Clear = 0x0C,
    Return = 0x0D,
    CapsLock = 0x14,
    Space = 0x20,
    PageUp = 0x21,
    PageDown = 0x22,
    End = 0x23,
    Home = 0x24,

    Period = 0xBE,
    Minus = 0xBD,
    Plus = 0xBB,
    Comma = 0xBC,

    Left = 0x25,
    Up = 0x26,
    Right = 0x27,
    Down = 0x28,

    Select = 0x29,
    Print = 0x2A,
    Snapshot = 0x2C,
    Insert = 0x2D,
    Delete = 0x2E,
    Help = 0x2F,

    Zero = 0x30,
    One = 0x31,
    Two = 0x32,
    Three = 0x33,
    Four = 0x34,
    Five = 0x35,
    Six = 0x36,
    Seven = 0x37,
    Eight = 0x38,
    Nine = 0x39,

    A = int32('A'),
    B = int32('B'),
    C = int32('C'),
    D = int32('D'),
    E = int32('E'),
    F = int32('F'),
    G = int32('G'),
    H = int32('H'),
    I = int32('I'),
    J = int32('J'),
    K = int32('K'),
    L = int32('L'),
    M = int32('M'),
    N = int32('N'),
    O = int32('O'),
    P = int32('P'),
    Q = int32('Q'),
    R = int32('R'),
    S = int32('S'),
    T = int32('T'),
    U = int32('U'),
    V = int32('V'),
    W = int32('W'),
    X = int32('X'),
    Y = int32('Y'),
    Z = int32('Z'),

    F1 = 0x70,
    F2 = 0x71,
    F3 = 0x72,
    F4 = 0x73,
    F5 = 0x74,
    F6 = 0x75,
    F7 = 0x76,
    F8 = 0x77,
    F9 = 0x78,
    F10 = 0x79,
    F11 = 0x7A,
    F12 = 0x7B,

    KP_NumLock = 0x90,
    KP_Divide = 0x6F,
    KP_Multiply = 0x6A,
    KP_Plus = 0x6B,
    KP_Minus = 0x6D,
    KP_Enter,
    KP_1 = 0x61,
    KP_2 = 0x62,
    KP_3 = 0x63,
    KP_4 = 0x64,
    KP_5 = 0x65,
    KP_6 = 0x66,
    KP_7 = 0x67,
    KP_8 = 0x68,
    KP_9 = 0x69,
    KP_0 = 0x60,
    KP_Period,
    KP_Separator = 0x6C,
    KP_Decimal = 0x6E,

    KApp = 0x5D,

    Mute = 0xAD,
    VolumeDown = 0xAE,
    VolumeUp = 0xAF,

    LeftShift = 0xA0,
    RightShift = 0xA1,

    LeftControl = 0xA2,
    RightControl = 0xA3,

    LeftAlt = 0xA4,
    RightAlt = 0xA5,
};

LICHT_CORE_API StringRef key_to_string(VirtualKey p_key);

class LICHT_CORE_API Input {
public:
    static Signal<const VirtualKey&> on_key_down;
    static Signal<const VirtualKey&> on_key_up;
    static Signal<const VirtualKey&> on_key_pressed;
    static Signal<const VirtualKey&> on_key_release;

    static Signal<const MouseMove&> on_mouse_move;
    static Signal<const MouseWheel&> on_mouse_wheel;

    static Signal<const Button&> on_button_down;
    static Signal<const Button&> on_button_up;
    static Signal<const Button&> on_button_pressed;
    static Signal<const Button&> on_button_release;

    static bool key_is_down(VirtualKey key);
    static bool key_is_up(VirtualKey key);
    static bool key_is_pressed(VirtualKey key);
    static bool key_is_release(VirtualKey key);

    static bool button_is_down(Button button);
    static bool button_is_up(Button button);
    static bool button_is_pressed(Button button);
    static bool button_is_release(Button button);

    static std::set<VirtualKey> keys_dow_;
    static std::set<VirtualKey> keys_up_;
    static Array<VirtualKey> keys_release_;
    static Array<VirtualKey> keys_pressed_;
    
    static std::set<Button> buttons_dow_;
    static std::set<Button> buttons_up_;
    static Array<Button> buttons_release_;
    static Array<Button> buttons_pressed_;
};

}  //namespace licht
