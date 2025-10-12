#pragma once

#include <functional>
#include <ostream>
#include "licht/core/containers/array.hpp"
#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"

namespace licht {

class WString;

LICHT_CORE_API size_t string_length(const char* str);
LICHT_CORE_API size_t string_length(const wchar_t* str);

LICHT_CORE_API errno_t string_copy(char* dst, size_t bytes_size, const char* src);
LICHT_CORE_API errno_t string_copy(wchar_t* dst, size_t bytes_size, const wchar_t* src);

LICHT_CORE_API errno_t string_cat(char* dst, size_t p_bytes_size, const char* src);
LICHT_CORE_API errno_t string_cat(wchar_t* dst, size_t p_bytes_size, const wchar_t* src);

LICHT_CORE_API int32 string_compare(const char* str1, const char* str2);
LICHT_CORE_API int32 string_compare(const wchar_t* str1, const wchar_t* str2);

/**
 * @brief Converts a UTF-8 encoded string to a wide character string (UTF-16).
 * This function allocates memory for the large string and returns it, which can cause a number of performance problems.
 *
 * @param c_str The UTF-8 encoded string to convert.
 * @return A WString containing the converted wide character string.
 */
LICHT_CORE_API WString unicode_of_str(const char* p_str);

template <typename CharType>
class StringBase {
public:
    size_t size() const {
        return buffer_.size() > 0 ? buffer_.size() - 1 : 0;
    }

    size_t capacity() const {
        return buffer_.capacity();
    }

    StringBase& append(const CharType* str) {
        LCHECK_MSG(str, "Appempt to append a c-string null in a String.");

        size_t len = string_length(str);
        if (buffer_.size() > 0) {
            buffer_.pop();  // Remove null terminator.
        }
        for (size_t i = 0; i < len; i++) {
            buffer_.append(str[i]);
        }
        buffer_.append(CharType('\0'));
        return *this;
    }

    StringBase& append(CharType c) {
        if (buffer_.size() > 0) {
            buffer_.pop();  // Remove null terminator.
        }
        buffer_.append(c);
        buffer_.append(CharType('\0'));
        return *this;
    }

    StringBase& append(const StringBase& string) {
        append(string.data());
        return *this;
    }

    const CharType* data() const {
        return buffer_.data();
    }

    CharType* data() {
        return buffer_.data();
    }

    void clear() {
        buffer_.clear();
        buffer_.append(CharType('\0'));
    }

    void resize(size_t size) {
        buffer_.resize(size + 1);
        buffer_[size] = CharType('\0');
    }

    void reserve(size_t capacity) {
        buffer_.reserve(capacity + 1);
    }

    bool empty() {
        return size() == 0;
    }

public:
    StringBase()
        : buffer_() {
        buffer_.append(CharType('\0'));
    }

    explicit StringBase(size_t capacity)
        : buffer_() {
        reserve(capacity);
        buffer_.append(CharType('\0'));
    }

    StringBase(const CharType* c_str)
        : buffer_() {
        LCHECK_MSG(c_str, "Appempt to append a c-string null in a String.");
        append(c_str);
    }

    StringBase(const StringBase& other)
        : buffer_(other.buffer_) {}

    StringBase& operator=(const StringBase& other) {
        if (this != std::addressof(other)) {
            buffer_ = other.buffer_;
        }
        return *this;
    }

    StringBase& operator+=(const CharType* c_str) {
        LCHECK_MSG(c_str, "Appempt to append a c-string null in a String.");
        return append(c_str);
    }

    StringBase& operator+=(const StringBase& str) {
        return append(str);
    }

    StringBase& operator+=(CharType c) {
        if (buffer_.size() > 0) {
            buffer_.pop();
        }
        buffer_.append(c);
        buffer_.append(CharType('\0'));
        return *this;
    }

    bool operator==(const StringBase& str) const {
        return string_compare(buffer_.data(), str.data()) == 0;
    }

    virtual ~StringBase() {}

protected:
    void ensure_capacity(size_t required_capacity) {
        if (required_capacity + 1 > buffer_.capacity()) {
            reserve(required_capacity);
        }
    }

protected:
    Array<CharType> buffer_;
};

class LICHT_CORE_API WString : public StringBase<wchar_t> {
public:
    WString()
        : StringBase<wchar_t>() {}

    explicit WString(size_t capacity)
        : StringBase<wchar_t>(capacity) {
    }

    WString(const wchar_t* str)
        : StringBase<wchar_t>(str) {
    }

    WString(const WString& other)
        : StringBase<wchar_t>(other) {}

    WString& operator=(const WString& other) {
        if (this != std::addressof(other)) {
            buffer_ = other.buffer_;
        }
        return *this;
    }
};

class LICHT_CORE_API String : public StringBase<char> {
public:
    String()
        : StringBase<char>() {
    }

    explicit String(size_t capacity)
        : StringBase<char>(capacity) {
    }

    String(const char* str)
        : StringBase<char>(str) {
    }

    String(const String& other)
        : StringBase<char>(other) {}

    String& operator=(const String& other) {
        if (this != std::addressof(other)) {
            buffer_ = other.buffer_;
        }
        return *this;
    }
};

}  //namespace licht

template <typename CharType>
std::ostream& operator<<(std::ostream& os, const licht::StringBase<CharType>& str) {
    os << str.data();
    return os;
}

template <>
struct std::hash<licht::String> {
    size_t operator()(const licht::String& s) const noexcept {
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