#pragma once

#include <functional>

#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"

namespace licht {

class LICHT_CORE_API Hasher {
public:
    template <typename T>
    static uint32 hash(const T& value) {
        return std::hash<T>{}(value);
    }
};

}  //namespace licht