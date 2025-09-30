#pragma once

#include <utility>
#include "licht/core/containers/array.hpp"

namespace licht {

template <typename Signature>
class CallableList {};

template <typename ReturnType, typename... ArgumentTypes>
class CallableList<ReturnType(ArgumentTypes...)> {};

template <typename... ArgumentTypes>
class CallableList<void(ArgumentTypes...)> {
public:
    using Callback = void (*)(void*, ArgumentTypes&&...);

    void append(Callback callback) {
        functions_.append(callback);
    }

    void invoke(ArgumentTypes... args) {
        for (Callback& callback : functions_) {
            callback(std::forward<ArgumentTypes>(args)...);
        }
    }

private:
    Array<Callback> functions_;
};

}  //namespace licht