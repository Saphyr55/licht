#pragma once

#include <cstdint>
#include <cstdlib>
#include <iostream>

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using float32 = float;
using float64 = double;

#define NOMINMAX

/**
 * Defines LDEBUG on debugging.
 */
#ifndef LDEBUG
#if defined(_DEBUG) || defined(DEBUG)
#define LDEBUG
#endif
#endif

/**
 *
 */
#if defined(_MSC_VER)
#define LDEBUGBREAK() __debugbreak()
#else
#define LDEBUGBREAK() __builtin_trap()
#endif

/**
 *
 */
#ifdef LDEBUG
#define LCHECK(expr)   \
    if (!(expr)) {     \
        LDEBUGBREAK(); \
    }
#else
#define LCHECK(expr)
#endif

/**
 *
 */
// TODO: Remove std::cout ...
#ifdef LDEBUG
#define LCHECK_MSG(expr, msg)       \
    if (!(expr)) {                  \
        ::std::cout << msg << "\n"; \
        LDEBUGBREAK();              \
    }
#else
#define LCHECK_MSG(expr, msg)
#endif

#define LDEPRECATED(version, msg)

#define LCRASH(msg)         \
    LCHECK_MSG(false, msg); \
    abort();

#define LICHT_CONCAT_INNER(a, b) a##b
#define LCONCAT(a, b) LICHT_CONCAT_INNER(a, b)

#define LICHT_ENUM_FLAGS_CUSTOM_DERIVED(EnumClassName, IntType)                               \
    inline EnumClassName operator|(EnumClassName a, EnumClassName b) {                        \
        return static_cast<EnumClassName>(static_cast<IntType>(a) | static_cast<IntType>(b)); \
    }                                                                                         \
    inline EnumClassName operator&(EnumClassName a, EnumClassName b) {                        \
        return static_cast<EnumClassName>(static_cast<IntType>(a) & static_cast<IntType>(b)); \
    }                                                                                         \
    inline EnumClassName& operator|=(EnumClassName& a, EnumClassName b) {                     \
        a = a | b;                                                                            \
        return a;                                                                             \
    }                                                                                         \
    inline EnumClassName& operator&=(EnumClassName& a, EnumClassName b) {                     \
        a = a & b;                                                                            \
        return a;                                                                             \
    }

#define LICHT_ENUM_FLAGS(EnumClassName) LICHT_ENUM_FLAGS_CUSTOM_DERIVED(EnumClassName, uint32)