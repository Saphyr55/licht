#pragma once

#include "core/string/string_ref.hpp"
#include "core/string/string.hpp"

namespace licht {

StringRef vformat(StringRef format, auto... args) {
    
    int size = snprintf(nullptr, 0, format.data(), args...);
    if (size < 0) {
        return "";
    }

    String buffer(size + 1);
    snprintf(buffer.data(), size + 1, format.data(), args...);

    return buffer.data();
}

}
