#ifdef _WIN32
#include "licht/core/platform/windows/windows.hpp"

#include "licht/core/platform/platform_file_system.hpp"
#include "licht/core/string/string.hpp"

namespace licht {

const char* platform_get_current_directory() {
    static char buffer[1024];
    DWORD ret = GetCurrentDirectoryA(MAX_PATH, buffer);
    if (ret == 0 || ret >= MAX_PATH) {
        buffer[0] = '\0';
    }
    return buffer;
}

int32 platform_create_directory(const char* directory_path) {
    if (!directory_path || directory_path[0] == '\0') return -1;

    String path(directory_path);

    for (size_t i = 1; i < path.size(); i++) {
        if (path[i] == '\\' || path[i] == '/') {
            char temp = path[i];
            path[i] = '\0';
            if (!CreateDirectoryA(path.data(), NULL)) {
                DWORD err = GetLastError();
                if (err != ERROR_ALREADY_EXISTS) return -1;
            }
            path[i] = temp;
        }
    }

    if (!CreateDirectoryA(path.data(), NULL)) {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS) return -1;
    }

    return 0;
}

}  //namespace licht

#endif