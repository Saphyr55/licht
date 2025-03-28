#pragma once

#include <cstdint>
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

#ifdef _WIN32
#undef min
#undef max
#undef ERROR
#undef DELETE
#undef MessageBox
#undef Error
#undef OK
#undef CONNECT_DEFERRED
#undef MemoryBarrier
#undef MONO_FONT
#endif

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

#define LDEPRECATED(msg)
