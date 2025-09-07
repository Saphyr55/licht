#pragma once

#include "licht/core/string/string_ref.hpp"
#include "licht/core/string/string.hpp"

namespace licht {

String vformat(StringRef format, auto... args) {
    
    String fmt(format.data());

    int32 size = ::snprintf(nullptr, 0, fmt.data(), args...);
    if (size < 0) {
        return String();
    }

    String buffer(size + 1);
    buffer.resize(size + 1);
    ::snprintf(buffer.data(), size + 1, fmt.data(), args...);

    return buffer;
}

}
