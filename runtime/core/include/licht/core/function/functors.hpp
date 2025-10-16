#pragma once

namespace licht {

inline auto map(auto* begin, auto* end, auto&& functor) -> void {
    for (auto* it = begin; it != end; it++) {
        functor(*it);
    }
}

inline auto reduce(auto* begin, auto* end, auto initiale_value, auto&& functor) -> auto {
    auto value = initiale_value; 
    for (auto* it = begin; it != end; it++) {
        value = functor(value, *it);
    }
    return value; 
}

}  //namespace licht