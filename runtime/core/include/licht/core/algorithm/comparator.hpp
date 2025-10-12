#pragma once

#include "licht/core/defines.hpp"

namespace licht {

enum class Ordering : int32 {
    Less = -1,
    Equal = 0,
    Greater = 1
};

template <typename T>
class Comparator {

    Ordering compare(const T& lhs, const T& rhs) const {
        
        if (lhs < rhs) {
            return Ordering::Less;
        }
        
        if (lhs > rhs) {
            return Ordering::Greater;
        }

        return Ordering::Equal;
    }

};

}