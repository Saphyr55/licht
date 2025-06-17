#include "licht/core/string/string.hpp"

#include <cstring>

namespace licht {

usize String::length(const char* cstr) {
    return ::strlen(cstr);
}

errno_t String::copy(char* dst, usize bytes_size, const char* src) {
    return ::strcpy_s(dst, bytes_size, src);
}

errno_t String::cat(char* dst, usize bytes_size, const char* src) {
    return ::strcat_s(dst, bytes_size, src);
}

int32 String::compare(const char* str1, const char* str2) {
    return ::strcmp(str1, str2);
}

wchar_t* String::unicode_of(const char* c_str) {
    usize size = length(c_str) + 1;
    wchar_t* c_wstr = new wchar_t[size];
    usize outSize;
    ::mbstowcs_s(&outSize, c_wstr, size, c_str, size - 1);
    return c_wstr;
}

}  //namespace licht
