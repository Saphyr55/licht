#pragma once

#include <type_traits>

namespace licht {

template <typename Signature>
class FunctionRef;

template <typename ReturnType, typename... ArgumentTypes>
class FunctionRef<ReturnType(ArgumentTypes...)> {
public:
    FunctionRef() = default;

    template <typename Callable,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<Callable>, FunctionRef>>>
    FunctionRef(Callable&& c) {
        obj_ = (void*)std::addressof(c);
        callback_ = [](void* obj, ArgumentTypes&&... args) -> ReturnType {
            return (*reinterpret_cast<std::add_pointer_t<Callable>>(obj))(
                std::forward<ArgumentTypes>(args)...);
        };
    }

    ReturnType operator()(ArgumentTypes... args) const {
        return callback_(obj_, std::forward<ArgumentTypes>(args)...);
    }

    explicit operator bool() const noexcept {
        return callback_ != nullptr;
    }

private:
    using Callback = ReturnType (*)(void*, ArgumentTypes&&...);

    void* obj_ = nullptr;
    Callback callback_ = nullptr;
};

template <typename ReturnType, typename... Args>
FunctionRef<ReturnType(Args...)> bind(ReturnType (*func)(Args...)) {
    return FunctionRef<ReturnType(Args...)>(func);
}

template <typename C, typename ReturnType, typename... Args>
FunctionRef<ReturnType(Args...)> bind(C& instance, ReturnType (C::*func)(Args...)) {
    return FunctionRef<ReturnType(Args...)>(
        [&instance, func](Args&&... args) -> ReturnType {
            return (instance.*func)(std::forward<Args>(args)...);
        });
}

template <typename C, typename ReturnType, typename... Args>
FunctionRef<ReturnType(Args...)> bind(const C& instance, ReturnType (C::*func)(Args...) const) {
    return FunctionRef<ReturnType(Args...)>(
        [&instance, func](Args&&... args) -> ReturnType {
            return (instance.*func)(std::forward<Args>(args)...);
        });
}

}  //namespace licht