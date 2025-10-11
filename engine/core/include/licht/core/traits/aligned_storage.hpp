#pragma once

#include "licht/core/defines.hpp"

namespace licht {

template <typename T>
struct AlignedStorage {
    static constexpr const size_t size = sizeof(T);
    static constexpr const size_t alignment = alignof(T);

    struct type {
        alignas(alignment) uint8 storage[size];
    };
};

template<typename T>
using AlignedStorageType = typename AlignedStorage<T>::type; 

}  //namespace licht