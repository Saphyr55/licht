#include "licht/core/string/string.hpp"

#include <cstring>

namespace licht {

size_t string_length(const char* cstr) {
    LCHECK(cstr)
    return ::strlen(cstr);
}

size_t string_length(const wchar_t* wcstr) {
    LCHECK(wcstr)
    return ::wcslen(wcstr);
}

errno_t string_copy(char* dst, size_t bytes_size, const char* src) {
    return ::strcpy_s(dst, bytes_size, src);
}

errno_t string_copy(wchar_t* dst, size_t bytes_size, const wchar_t* src) {
    return ::wcscpy_s(dst, bytes_size, src);
}

errno_t string_cat(char* dst, size_t bytes_size, const char* src) {
    return ::strcat_s(dst, bytes_size, src);
}

errno_t string_cat(wchar_t* dst, size_t bytes_size, const wchar_t* src) {
    return ::wcscat_s(dst, bytes_size, src);
}

int32 string_compare(const char* str1, const char* str2) {
    return ::strcmp(str1, str2);
}

int32 string_compare(const wchar_t* str1, const wchar_t* str2) {
    return ::wcscmp(str1, str2);
}

WString unicode_of_str(const char* c_str) {
    size_t size = string_length(c_str) + 1;
    WString wstring(size);
    size_t outSize;
    ::mbstowcs_s(&outSize, wstring.data(), size, c_str, size - 1);
    return wstring;
}

}  //namespace licht
