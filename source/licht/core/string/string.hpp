#pragma once

#include "licht/core/collection/array.hpp"
#include "licht/core/core_exports.hpp"

namespace licht {

class WString;

LICHT_CORE_API usize string_length(const char* p_str);
LICHT_CORE_API usize string_length(const wchar_t* p_str);

LICHT_CORE_API errno_t string_copy(char* p_dst, usize p_bytes_size, const char* p_src);
LICHT_CORE_API errno_t string_copy(wchar_t* p_dst, usize p_bytes_size, const wchar_t* p_src);

LICHT_CORE_API errno_t string_cat(char* p_dst, usize p_bytes_size, const char* p_src);
LICHT_CORE_API errno_t string_cat(wchar_t* p_dst, usize p_bytes_size, const wchar_t* p_src);

LICHT_CORE_API int32 string_compare(const char* p_str1, const char* p_str2);
LICHT_CORE_API int32 string_compare(const wchar_t* p_str1, const wchar_t* p_str2);

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

    void reverse(usize p_capacity) {
        buffer_.reverse(p_capacity);
    }

public:
    StringBase()
        : buffer_() {
        buffer_.append('\0');
    }

    explicit StringBase(usize p_capacity)
        : buffer_(p_capacity) {
        LCHECK(p_capacity != 0)
        buffer_.append('\0');
    }

    StringBase(const CharType* p_str)
        : buffer_(string_length(p_str) + 1) {
        append(p_str);
    }

    StringBase(const StringBase& p_other)
        : buffer_(p_other.buffer_) {}

    StringBase& operator=(const StringBase& p_other) {
        if (this != std::addressof(p_other)) {
            buffer_ = p_other.buffer_;
        }
        return *this;
    }

    virtual ~StringBase() {}

protected:
    void ensure_capacity(usize p_required_capacity) {
        if (p_required_capacity > buffer_.capacity()) {
            reverse(p_required_capacity);
        }
    }

protected:
    Array<CharType> buffer_;
};

class WString : public StringBase<wchar_t> {
public:
    WString() : StringBase<wchar_t>() { }

    explicit WString(usize p_capacity)
        : StringBase<wchar_t>(p_capacity) {
    }

    WString(const wchar_t* p_str)
        : StringBase<wchar_t>(string_length(p_str) + 1) {
    }

    WString(const WString& p_other)
        : StringBase<wchar_t>(p_other) {}

    WString& operator=(const WString& p_other) {
        if (this != std::addressof(p_other)) {
            buffer_ = p_other.buffer_;
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

    explicit String(usize p_capacity)
        : StringBase<char>(p_capacity) {
    }

    String(const char* p_str)
        : StringBase<char>(string_length(p_str) + 1) {
    }

    String(const String& p_other)
        : StringBase<char>(p_other) {}

    String& operator=(const String& p_other) {
        if (this != std::addressof(p_other)) {
            buffer_ = p_other.buffer_;
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
