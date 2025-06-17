#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/memory/memory.hpp"

namespace licht {

template <typename T>
class Option {
public:
    constexpr static Option<T> some(const T& value) { return Option<T>(value); }

    constexpr static Option<T> none() { return Option<T>(); }

    constexpr Option(const T& value)
        : is_some_(true) {
        ::new (&storage_) T(value);
    }

    constexpr Option()
        : is_some_(false) {}

    constexpr Option(const Option& other)
        : is_some_(other.is_some_) {
        if (is_some_) {
            ::new (&storage_) T(*other.data());
        }
    }

    constexpr Option& operator=(const Option& other) {
        if (this != &other) {
            if (is_some_) {
                data()->~T();
            }
            is_some_ = other.is_some_;
            if (is_some_) {
                ::new (&storage_) T(*other.data());
            }
        }
        return *this;
    }

    constexpr ~Option() {
        if (is_some_) {
            data()->~T();
        }
    }

    constexpr bool has_value() const { return is_some_; }

    constexpr T unwrap() const {
        LCHECK(has_value())
        return *data();
    }

    constexpr T unwrap_or(const T& default_value) const {
        return has_value() ? *data() : default_value;
    }

    const T& operator*() const {
        LCHECK(has_value())
        return *data();
    }

    T& operator*() {
        LCHECK(has_value())
        return *data();
    }

private:
    const T* data() const { return reinterpret_cast<const T*>(&storage_); }

    T* data() { return reinterpret_cast<T*>(&storage_); }

private:
    alignas(T) char storage_[sizeof(T)];
    bool is_some_;
};

}  // namespace licht