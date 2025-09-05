#pragma once

#include <concepts>

namespace licht {

template <typename Signature>
class FunctionRef;

template <typename R, typename... Args>
class FunctionRef<R(Args...)> {
public:
    R operator()(Args... args) const {
        return callback_(obj_ptr_, std::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept {
        return callback_ != nullptr;
    }

public:
    FunctionRef() = default;

    FunctionRef(std::invocable<R, Args...> auto&& callable)
        : obj_ptr_(reinterpret_cast<void*>(&callable)), callback_(&invoke<decltype(callable)>) {
    }

    FunctionRef(const FunctionRef&) = default;
    FunctionRef& operator=(const FunctionRef&) = default;

private:
    template <typename Callable>
    static R invoke(void* obj, Args&&... args) {
        return (*reinterpret_cast<Callable*>(obj))(std::forward<Args>(args)...);
    }

private:
    void* obj_ptr_ = nullptr;

    R (*callback_)(void*, Args&&...) = nullptr;
};

}  //namespace licht