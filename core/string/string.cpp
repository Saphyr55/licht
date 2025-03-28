#include "core/string/string.hpp"

#include <cstddef>
#include <cstdio>
#include <cstring>

namespace licht {

size_t String::length(const char* cstr) {
    return strlen(cstr);
}

errno_t String::copy(char* dst, size_t bytes_size, const char* src) {
    return strcpy_s(dst, bytes_size, src);
}

errno_t String::cat(char* dst, size_t bytes_size, const char* src) {
    return strcat_s(dst, bytes_size, src);
}

int32 String::compare(const char* str1, const char* str2) {
    return strcmp(str1, str2);
}

const wchar_t* String::unicode_of(const char* c) {
    const size_t cSize = length(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);
    return wc;
}

}  //namespace licht
