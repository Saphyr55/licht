#pragma once

#include <concepts>

#include "licht/core/string/string.hpp"

namespace licht {

template <typename CharType>
class StringRefBase {
public:
    constexpr StringRefBase()
        : data_(nullptr) {}

    constexpr StringRefBase(const CharType* str)
        : data_(str) {}

    constexpr StringRefBase(const String& str) 
        requires std::same_as<CharType, char>
        : data_(str.data()) {}

    constexpr ~StringRefBase() = default;

    constexpr StringRefBase(const StringRefBase& other) = default;

    constexpr StringRefBase& operator=(const StringRefBase& other) = default;

    constexpr const CharType& operator[](size_t index) const {
        LCHECK(index >= size())
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

template <>
struct LICHT_CORE_API std::hash<::licht::StringRef> {
    size_t operator()(const licht::StringRef& s) const noexcept {
        return std::hash<const char*>{}(s.data());
    }
};
