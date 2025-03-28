#pragma once

#include "core/defines.hpp"

namespace licht {

enum class Button : uint8 {
    Left = 1,
    Middle = 2,
    Right = 3,
    XButton1 = 4,
    XButton2 = 5
};

enum class Key : uint32 {
    None,

    Back = 0x08,
    Tab = 0x09,
    Clear = 0x0C,
    Return = 0x0D,
    CapsLock = 0x14,
    Space = 0x20,
    PageUp = 0x20,
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

    KP_NUMLOCK = 0x90,
    KP_DIVIDE = 0x6F,
    KP_MULTIPLY = 0x6A,
    KP_MINUS = 0x6D,
    KP_PLUS = 0x6B,
    KP_ENTER,
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
    KP_PERIOD,
    KP_SEPARATOR = 0x6C,
    KP_DECIMAL = 0x6E,

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

}  //namespace licht
