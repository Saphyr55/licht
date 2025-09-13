#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/core_exports.hpp"

namespace licht {

class WString;

LICHT_CORE_API usize string_length(const char* str);
LICHT_CORE_API usize string_length(const wchar_t* str);

LICHT_CORE_API errno_t string_copy(char* dst, usize bytes_size, const char* src);
LICHT_CORE_API errno_t string_copy(wchar_t* dst, usize bytes_size, const wchar_t* src);

LICHT_CORE_API errno_t string_cat(char* dst, usize p_bytes_size, const char* src);
LICHT_CORE_API errno_t string_cat(wchar_t* dst, usize p_bytes_size, const wchar_t* src);

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
    usize size() const {
        return buffer_.size();
    }

    usize capacity() const {
        return buffer_.capacity();
    }

    StringBase& append(const char* p_str) {
        usize size = string_length(p_str);
        buffer_.pop();
        for (int i = 0; i < size; i++) {
            buffer_.append(p_str[i]);
        }
        buffer_.append('\0');
        return *this;
    }

    StringBase& append(const StringBase& p_string) {
        append(p_string.data());
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
    }

    void resize(usize size) {
        buffer_.resize(size);
    }

    void reserve(usize capacity) {
        buffer_.reserve(capacity);
    }

public:
    StringBase()
        : buffer_() {
        buffer_.append('\0');
    }

    explicit StringBase(usize capacity)
        : buffer_(capacity) {
        LCHECK(capacity != 0)
        buffer_.append('\0');
    }

    StringBase(const CharType* str)
        : buffer_(string_length(str) + 1) {
        append('\0'); // Will be removed with the second append.
        append(str);
    }

    StringBase(const StringBase& other)
        : buffer_(other.buffer_) {}

    StringBase& operator=(const StringBase& other) {
        if (this != std::addressof(other)) {
            buffer_ = other.buffer_;
        }
        return *this;
    }

    virtual ~StringBase() {}

protected:
    void ensure_capacity(usize required_capacity) {
        if (required_capacity > buffer_.capacity()) {
            reserve(required_capacity);
        }
    }

protected:
    Array<CharType> buffer_;
};

class WString : public StringBase<wchar_t> {
public:
    WString() : StringBase<wchar_t>() { }

    explicit WString(usize capacity)
        : StringBase<wchar_t>(capacity) {
    }

    WString(const wchar_t* str)
        : StringBase<wchar_t>(string_length(str) + 1) {
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

class String : public StringBase<char> {
public:
    String()
        : StringBase<char>() {
        buffer_.append('\0');
    }

    explicit String(usize capacity)
        : StringBase<char>(capacity) {
    }

    String(const char* str)
        : StringBase<char>(string_length(str) + 1) {
    }

    String(const char* str, usize size)
        : StringBase<char>(size) {
        append(str);
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
struct LICHT_CORE_API std::hash<licht::String> {
    usize operator()(const licht::String& s) const noexcept {
        return std::hash<const char*>{}(s.data());
    }
};
