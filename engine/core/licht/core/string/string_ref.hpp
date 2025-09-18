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

    constexpr StringRefBase(const CharType* str)
        : data_(str) {
    }

    constexpr StringRefBase(const String& str) 
        requires std::same_as<CharType, char>
        : data_(str.data()) {}

    constexpr ~StringRefBase() = default;

    constexpr StringRefBase(const StringRefBase& other) = default;

    constexpr StringRefBase& operator=(const StringRefBase& other) = default;

    constexpr const CharType& operator[](usize index) const {
        LCHECK(index < size())
        return data_[index];
    }

    constexpr operator const CharType*() const { return data_; }

    constexpr const CharType* data() const { return data_; }

    constexpr usize size() const { return string_length(data_); }

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

template <typename CharType>
std::ostream& operator<<(std::ostream& os, const licht::StringRefBase<CharType>& str) {
    os << str.data();
    return os;
}

template <>
struct LICHT_CORE_API std::hash<::licht::StringRef> {
    usize operator()(const licht::StringRef& s) const noexcept {
        const char* data = s.data();
        usize len = s.size();

        // FNV-1a 64-bit
        uint64 hash = 14695981039346656037ULL;
        constexpr uint64_t prime = 1099511628211ULL;

        for (usize i = 0; i < len; ++i) {
            hash ^= static_cast<uint8>(data[i]);
            hash *= prime;
        }

        return static_cast<usize>(hash);
    }
};
