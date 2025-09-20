#pragma once

#include <basetsd.h>
namespace licht {

enum class Ordering : INT32 {
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