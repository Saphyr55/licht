#include "licht/core/platform/platform_file_system.hpp"

namespace licht {

void platform_get_directory(const char *filepath, char *dir_out, size_t size) {

    if (!filepath || !dir_out || size == 0) {
        if (dir_out && size > 0) dir_out[0] = '\0';
        return;
    }

    const char *last_slash = NULL;
    const char *p = filepath;

    while (*p) {
        if (*p == '/' || *p == '\\') {
            last_slash = p;
        }
        p++;
    }

    if (last_slash) {
        size_t dir_len = last_slash - filepath;
        if (dir_len >= size) {
            dir_len = size - 1;
        }
        
        string_copy(dir_out, dir_len, filepath);
        
        dir_out[dir_len] = '\0';
    } else {
        dir_out[0] = '\0';
    }
}


    
}