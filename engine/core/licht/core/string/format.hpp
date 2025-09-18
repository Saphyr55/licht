#pragma once

#include "licht/core/string/string_ref.hpp"
#include "licht/core/string/string.hpp"

#include <cstdarg>

namespace licht {

template <typename... Args>
inline String vformat(StringRef fmt, Args&&... args) {
    va_list ap;
    int32 size = ::snprintf(nullptr, 0, fmt, std::forward<Args>(args)...);

    if (size < 0) {
        return String("");
    }

    String buffer;
    buffer.resize(size + 1);
    ::snprintf(buffer.data(), buffer.size(), fmt, std::forward<Args>(args)...);

    return buffer;
}

}
