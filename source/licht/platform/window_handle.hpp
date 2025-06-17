#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/platform/platform_exports.hpp"

namespace licht {

using WindowHandle = uint32;

struct LICHT_PLATFORM_API WindowStatues {
    StringRef title;
    float32 width;
    float32 height;
    float32 x;
    float32 y;
};

}