#ifdef _WIN32

#include <Windows.h>

#include "licht/core/platform/platform_file_system.hpp"

namespace licht {

const char* platform_get_current_directory() {
    static char buffer[1024];
    DWORD ret = GetCurrentDirectoryA(MAX_PATH, buffer);
    if (ret == 0 || ret >= MAX_PATH) {
        buffer[0] = '\0';
    }
    return buffer;
}

}

#endif