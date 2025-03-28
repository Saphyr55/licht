#pragma once

#include <functional>

#include "core/core_exports.hpp"
#include "core/defines.hpp"

namespace licht {

class LICHT_CORE_API Hasher {
public:
    static unsigned long long fnv_hash_1a_64(void* key, int32 len) {
        uint8* p = static_cast<uint8*>(key);
        uint64 h = 0xcbf29ce484222325ULL;
    
        for (int32 i = 0; i < len; i++) {
            h = (h ^ p[i]) * 0x100000001b3ULL;
        }
    
        return h;
    }
    
    template <typename T>
    static uint32 hash(const T& value) {
        return std::hash<T>{}(value);
    }

};

}  //namespace licht