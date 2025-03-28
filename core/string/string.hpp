#pragma once

#include "core/collection/array.hpp"
#include "core/core_exports.hpp"

namespace licht {

class LICHT_CORE_API String {
public:
    static size_t length(const char* str);
    static errno_t copy(char* dst, size_t bytes_size, const char* src);
    static errno_t cat(char* dst, size_t bytes_size, const char* src);
    static int32 compare(const char* str1, const char* str2);
    static const wchar_t* unicode_of(const char* c);

public:
    size_t size() const {
        return buffer_.size();
    }

    size_t capacity() const {
        return buffer_.capacity();
    }

    String& append(const char* str) {
        size_t size = length(str);
        buffer_.pop();
        for (int i = 0; i < size; i++) {
            buffer_.append(str[i]);
        }
        buffer_.append('\0');
        return *this;
    }

    String& append(const String& str) {
        append(str.data());
        return *this;
    }

    const char* data() const {
        return buffer_.data();
    }

    char* data() {
        return buffer_.data();
    }

    void clear() {
        buffer_.clear();
    }

    void reverse(size_t capacity) {
        buffer_.reverse(capacity);
    }

public:
    String()
        : buffer_() {
        buffer_.append('\0');
    }

    explicit String(size_t capacity)
        : buffer_(capacity) {
        LCHECK(capacity != 0)
        buffer_.append('\0');
    }

    String(const char* str)
        : buffer_(length(str) + 1) {
        append(str);
    }

    String(const String& other)
        : buffer_(other.buffer_) {}

    String& operator=(const String& other) {
        if (this != std::addressof(other)) {
            buffer_ = other.buffer_;
        }
        return *this;
    }

    ~String() {}

protected:
    void ensure_capacity(size_t required_capacity) {
        if (required_capacity > buffer_.capacity()) {
            reverse(required_capacity);
        }
    }

private:
    Array<char> buffer_;
};

}  //namespace licht

template <>
struct LICHT_CORE_API std::hash<licht::String> {
    size_t operator()(const licht::String& s) const noexcept {
        return std::hash<const char*>{}(s.data());
    }
};
