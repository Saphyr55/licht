#pragma once

#include "licht/core/string/string_ref.hpp"
#include "licht/core/string/string.hpp"

#include <cstdarg>

namespace licht {

inline String vformat(StringRef fmt, auto&&... args) {
    va_list ap;
    int32 size = ::snprintf(nullptr, 0, fmt, std::forward<decltype(args)>(args)...);

    if (size < 0) {
        return String();
    }

    String buffer(size + 1);
    buffer.resize(size + 1);
    ::snprintf(buffer.data(), buffer.size(), fmt, std::forward<decltype(args)>(args)...);

    return buffer;
}
}
