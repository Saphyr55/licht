#pragma once

#include <concepts>
#include <functional>
#include <ostream>

#include "licht/core/defines.hpp"
#include "licht/core/string/string.hpp"

namespace licht {

template <typename CharType>
class StringRefBase {
public:
    constexpr StringRefBase()
        : data_("") {}

    constexpr StringRefBase(const CharType* data)
        : data_(data) {
        LCHECK_MSG(data, "StringRefBase is not nullable.");
    }

    constexpr StringRefBase(const String& str)
        requires std::same_as<CharType, char>
        : data_(str.data()) {
        LCHECK_MSG(str.data(), "StringRefBase is not nullable.");
    }

    constexpr ~StringRefBase() = default;

    constexpr StringRefBase(const StringRefBase& other) = default;

    constexpr StringRefBase& operator=(const StringRefBase& other) = default;

    constexpr const CharType& operator[](size_t index) const {
        LCHECK(index < size());
        return data_[index];
    }

    constexpr operator const CharType*() const { return data_; }

    constexpr const CharType* data() const { return data_; }

    constexpr size_t size() const { return string_length(data_); }

    constexpr bool empty() const { return size() == 0; }

    constexpr bool operator==(const StringRefBase& other) const {
        return size() == other.size() && string_compare(data_, other.data_) == 0;
    }

    constexpr bool operator==(const CharType* str) const {
        return *this == StringRefBase(str);
    }


private:
    const CharType* data_;
};

using StringRef = StringRefBase<char>;
using WStringRef = StringRefBase<wchar_t>;

}  // namespace licht

template<typename CharType>
constexpr licht::StringBase<CharType> operator+(licht::StringRefBase<CharType> lhs, licht::StringRefBase<CharType> rhs) {
    licht::StringBase<CharType> base(lhs);
    base.append(rhs);
    return std::move(base);
}

template<typename CharType>
constexpr licht::StringBase<CharType> operator+(licht::StringRefBase<CharType> lhs, const CharType* rhs) {
    licht::StringBase<CharType> base(lhs);
    base.append(rhs);
    return std::move(base);
}

template<typename CharType>
constexpr licht::StringBase<CharType> operator+(const CharType* lhs, licht::StringRefBase<CharType> rhs) {
    licht::StringBase<CharType> base(lhs);
    base.append(rhs);
    return std::move(base);
}

template <typename CharType>
std::ostream& operator<<(std::ostream& os, const licht::StringRefBase<CharType>& str) {
    os << str.data();
    return os;
}

template <>
struct LICHT_CORE_API std::hash<::licht::StringRef> {
    size_t operator()(const licht::StringRef& s) const noexcept {
        const char* data = s.data();
        size_t len = s.size();

        // FNV-1a 64-bit
        uint64 hash = 14695981039346656037ULL;
        constexpr uint64_t prime = 1099511628211ULL;

        for (size_t i = 0; i < len; ++i) {
            hash ^= static_cast<uint8>(data[i]);
            hash *= prime;
        }

        return static_cast<size_t>(hash);
    }
};
