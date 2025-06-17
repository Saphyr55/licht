#pragma once

#include "licht/core/string/string_ref.hpp"
#include "licht/core/string/string.hpp"

namespace licht {

StringRef vformat(StringRef format, auto... args) {
    
    int32 size = ::snprintf(nullptr, 0, format.data(), args...);
    if (size < 0) {
        return StringRef();
    }

    String buffer(size + 1);
    ::snprintf(buffer.data(), size + 1, format.data(), args...);

    return buffer.data();
}

}
