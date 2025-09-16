#pragma once

#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

namespace licht {

template <typename T>
class Success;

template <typename T>
class Failure;

template <typename SuccessT, typename FailureT>
class Result;

struct FailureType {
    explicit FailureType() = default;
};

namespace internal {

static constexpr FailureType failure_internal{};

template <typename T, typename U>
constexpr bool operator==(const Success<T>& lhs,
                          const Success<U>& rhs) noexcept {
    if constexpr (!std::is_same_v<T, U>) {
        return false;
    } else {
        return lhs.value() == rhs.value();
    }
}

template <typename T, typename U>
constexpr bool operator!=(const Success<T>& lhs,
                          const Success<U>& rhs) noexcept {
    return !(lhs == rhs);
}

template <typename Exp, typename F>
constexpr auto and_then(Exp&& exp, F&& f) noexcept {

    using T = std::remove_cvref_t<decltype(exp.value())>;
    using E = std::remove_cvref_t<decltype(exp.error())>;

    auto invoke_f = [&]() -> decltype(auto) {
        if constexpr (!std::is_void_v<T>) {
            return std::invoke(std::forward<F>(f), std::forward<Exp>(exp).value());
        } else {
            return std::invoke(std::forward<F>(f));
        }
    };

    using R = decltype(invoke_f());

    return exp.has_value() ? invoke_f()
                           : R(failure_internal, std::forward<Exp>(exp).error());
}

template <typename Exp, typename F>
constexpr auto or_else(Exp&& exp, F&& f) noexcept {
    using T = std::remove_cvref_t<decltype(exp.value())>;
    using G = std::invoke_result_t<F, decltype(std::forward<Exp>(exp).error())>;

    if (!exp.has_value()) {
        return std::invoke(std::forward<F>(f), std::forward<Exp>(exp).error());
    }

    if constexpr (!std::is_void_v<T>) {
        return G(std::in_place, std::forward<Exp>(exp).value());
    } else {
        return G();
    }
}

template <typename Exp, typename F>
constexpr auto map(Exp&& exp, F&& f) noexcept {
    using T = std::remove_cvref_t<decltype(exp.value())>;
    using E = std::remove_cvref_t<decltype(exp.Error())>;

    auto invoke_f = [&]() -> decltype(auto) {
        if constexpr (!std::is_void_v<T>) {
            return std::invoke(std::forward<F>(f), std::forward<Exp>(exp).value());
        } else {
            return std::invoke(std::forward<F>(f));
        }
    };

    using U = std::remove_cv_t<decltype(invoke_f())>;

    if (!exp.has_value()) {
        return Result<U, E>(failure_internal, std::forward<Exp>(exp).error());
    }

    if constexpr (!std::is_void_v<U>) {
        return Result<U, E>(std::in_place, invoke_f());
    } else {
        invoke_f();
        return Result<U, E>();
    }
}

template <typename Exp, typename F>
constexpr auto map_error(Exp&& exp, F&& f) noexcept {
    using T = std::remove_cvref_t<decltype(exp.value())>;
    using G = std::remove_cv_t<
        std::invoke_result_t<F, decltype(std::forward<Exp>(exp).error())>>;

    if (!exp.has_value()) {
        return Result<T, G>(
            failure_internal,
            std::invoke(std::forward<F>(f), std::forward<Exp>(exp).error()));
    }

    if constexpr (std::is_void_v<T>) {
        return Result<T, G>();
    } else {
        return Result<T, G>(std::in_place, std::forward<Exp>(exp).value());
    }
}
}  // namespace internal

template <typename T>
class Success final {
public:
    template <typename U = T>
        requires(!std::is_same_v<std::remove_cvref_t<U>, Success<T>>) &&
                (!std::is_same_v<std::remove_cvref_t<U>, std::in_place_t>) &&
                std::is_constructible_v<T, U>
    constexpr explicit Success(U&& val) noexcept
        : value_(std::forward<U>(val)) {}

    template <typename... Args>
    constexpr explicit Success(std::in_place_t, Args&&... args) noexcept
        : value_(std::forward<Args>(args)...) {}

    template <typename U, typename... Args>
    constexpr explicit Success(std::in_place_t,
                               std::initializer_list<U> il,
                               Args&&... args) noexcept
        : value_(il, std::forward<Args>(args)...) {}

    constexpr T& value() & noexcept { return value_; }

    constexpr const T& value() const& noexcept { return value_; }

    constexpr T&& value() && noexcept { return std::move(value()); }

    constexpr const T&& value() const&& noexcept { return std::move(value()); }

    constexpr void swap(Success& other) noexcept {
        std::swap(value(), other.value());
    }

    friend constexpr void swap(Success& x, Success& y) noexcept { x.swap(y); }

private:
    T value_;
};

template <typename T>
    requires(std::is_void_v<T>)
class Success<T> final {
public:
    constexpr explicit Success() noexcept = default;
};

template <typename E>
class Failure final {
public:
    template <typename Err = E>
        requires(!std::is_same_v<std::remove_cvref_t<Err>, Failure<E>>) &&
                (!std::is_same_v<std::remove_cvref_t<Err>, std::in_place_t>) &&
                std::is_constructible_v<E, Err>
    constexpr explicit Failure(Err&& err) noexcept
        : error_(std::forward<Err>(err)) {}

    template <typename... Args>
    constexpr explicit Failure(std::in_place_t, Args&&... args) noexcept
        : error_(std::forward<Args>(args)...) {}

    template <typename U, typename... Args>
    constexpr explicit Failure(std::in_place_t,
                               std::initializer_list<U> il,
                               Args&&... args) noexcept
        : error_(il, std::forward<Args>(args)...) {}

    constexpr E& error() & noexcept { return error_; }

    constexpr const E& error() const& noexcept { return error_; }

    constexpr E&& error() && noexcept { return std::move(error()); }

    constexpr const E&& error() const&& noexcept { return std::move(error()); }

    constexpr void swap(Failure& other) noexcept {
        std::swap(error(), other.error());
    }

    friend constexpr void swap(Failure& x, Failure& y) noexcept { x.swap(y); }

private:
    E error_;
};

template <typename T, typename E>
class Result {
public:
    static constexpr size_t kValIdx = 1;
    static constexpr size_t kErrIdx = 2;
    static constexpr std::in_place_index_t<1> kValTag{};
    static constexpr std::in_place_index_t<2> kErrTag{};

    template <typename U, typename G>
    friend class Result;

    constexpr static Result<T, E> Success(const T& value) {
        return Result<T, E>(kValTag, value);
    }

    constexpr static Result<T, E> Failure(const E& error) {
        return Result<T, E>(kErrTag, error);
    }

    constexpr Result() noexcept
        requires(std::default_initializable<T>)
        : data_(kValTag) {}

    constexpr Result(const Result& rhs) noexcept
        : data_(rhs.data_) {}

    constexpr Result(Result&& rhs) noexcept
        : data_(std::move(rhs.data_)) {
        rhs.set_is_moved_from();
    }

    template <typename U, typename G>
    constexpr explicit Result(const Result<U, G>& rhs) noexcept {
        if (rhs.has_value()) {
            emplace_value(rhs.value());
        } else {
            emplace_error(rhs.error());
        }
    }

    template <typename U, typename G>
    constexpr explicit Result(Result<U, G>&& rhs) noexcept {
        if (rhs.has_value()) {
            emplace_value(std::move(rhs.value()));
        } else {
            emplace_error(std::move(rhs.error()));
        }
        rhs.set_is_moved_from();
    }

    template <typename... Args>
    constexpr explicit Result(decltype(kValTag), Args&&... args) noexcept
        : data_(kValTag, std::forward<Args>(args)...) {}

    template <typename U, typename... Args>
    constexpr explicit Result(decltype(kValTag),
                              std::initializer_list<U> il,
                              Args&&... args) noexcept
        : data_(kValTag, il, std::forward<Args>(args)...) {}

    template <typename... Args>
    constexpr explicit Result(decltype(kErrTag), Args&&... args) noexcept
        : data_(kErrTag, std::forward<Args>(args)...) {}

    template <typename U, typename... Args>
    constexpr explicit Result(decltype(kErrTag),
                              std::initializer_list<U> il,
                              Args&&... args) noexcept
        : data_(kErrTag, il, std::forward<Args>(args)...) {}

    constexpr Result& operator=(const Result& rhs) noexcept {
        data_ = rhs.data_;
        return *this;
    }

    constexpr Result& operator=(Result&& rhs) noexcept {
        data_ = std::move(rhs.data_);
        rhs.set_is_moved_from();
        return *this;
    }

    template <typename... Args>
    constexpr T& emplace_value(Args&&... args) noexcept {
        return data_.template emplace<kValIdx>(std::forward<Args>(args)...);
    }

    template <typename U, typename... Args>
    constexpr T& emplace_value(std::initializer_list<U> il,
                              Args&&... args) noexcept {
        return data_.template emplace<kValIdx>(il, std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr E& emplace_error(Args&&... args) noexcept {
        return data_.template emplace<kErrIdx>(std::forward<Args>(args)...);
    }

    template <typename U, typename... Args>
    constexpr E& emplace_error(std::initializer_list<U> il,
                              Args&&... args) noexcept {
        return data_.template emplace<kErrIdx>(il, std::forward<Args>(args)...);
    }

    void swap(Result& rhs) noexcept { data_.swap(rhs.data_); }

    constexpr bool has_value() const noexcept { return data_.index() == kValIdx; }

    constexpr T& value() noexcept { return std::get<kValIdx>(data_); }

    constexpr const T& value() const noexcept {
        return std::get<kValIdx>(data_);
    }

    constexpr E& error() noexcept { return std::get<kErrIdx>(data_); }

    constexpr const E& error() const noexcept {
        return std::get<kErrIdx>(data_);
    }

    template <typename F>
        requires(std::copy_constructible<E>)
    constexpr auto and_then(F&& f) & noexcept {
        return internal::and_then(*this, std::forward<F>(f));
    }

    template <typename F>
        requires(std::copy_constructible<E>)
    constexpr auto and_then(F&& f) const& noexcept {
        return internal::and_then(*this, std::forward<F>(f));
    }

    template <typename F>
        requires(std::move_constructible<E>)
    constexpr auto and_then(F&& f) && noexcept {
        return internal::and_then(std::move(*this), std::forward<F>(f));
    }

    template <typename F>
        requires(std::move_constructible<E>)
    constexpr auto and_then(F&& f) const&& noexcept {
        return internal::and_then(std::move(*this), std::forward<F>(f));
    }

    template <typename F>
    constexpr auto or_else(F&& f) & noexcept {
        return internal::or_else(*this, std::forward<F>(f));
    }

    template <typename F>
    constexpr auto or_else(F&& f) const& noexcept {
        return internal::or_else(*this, std::forward<F>(f));
    }

    template <typename F>
    constexpr auto or_else(F&& f) && noexcept {
        return internal::or_else(std::move(*this), std::forward<F>(f));
    }

    template <typename F>
    constexpr auto or_else(F&& f) const&& noexcept {
        return internal::or_else(std::move(*this), std::forward<F>(f));
    }

    template <typename F>
        requires(std::copy_constructible<E>)
    constexpr auto map(F&& f) & noexcept {
        return internal::map(*this, std::forward<F>(f));
    }

    template <typename F>
        requires(std::copy_constructible<E>)
    constexpr auto map(F&& f) const& noexcept {
        return internal::map(*this, std::forward<F>(f));
    }

    template <typename F>
        requires(std::move_constructible<E>)
    constexpr auto map(F&& f) && noexcept {
        return internal::map(std::move(*this), std::forward<F>(f));
    }

    template <typename F>
        requires(std::move_constructible<E>)
    constexpr auto map(F&& f) const&& noexcept {
        return internal::map(std::move(*this), std::forward<F>(f));
    }

    template <typename F>
    constexpr auto map_error(F&& f) & noexcept {
        return internal::map_error(*this, std::forward<F>(f));
    }

    template <typename F>
    constexpr auto map_error(F&& f) const& noexcept {
        return internal::map_error(*this, std::forward<F>(f));
    }

    template <typename F>
    constexpr auto map_error(F&& f) && noexcept {
        return internal::map_error(std::move(*this), std::forward<F>(f));
    }

    template <typename F>
    constexpr auto map_error(F&& f) const&& noexcept {
        return internal::map_error(std::move(*this), std::forward<F>(f));
    }

private:
    static constexpr size_t kNulIdx = 0;
    static_assert(kNulIdx != kValIdx);
    static_assert(kNulIdx != kErrIdx);

    constexpr bool is_moved_from() const noexcept {
        return data_.index() == kNulIdx;
    }

    constexpr void set_is_moved_from() noexcept {
        data_.template emplace<kNulIdx>();
    }

    std::variant<std::monostate, T, E> data_;
};

}  // namespace licht