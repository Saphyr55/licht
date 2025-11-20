#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/string/string.hpp"

namespace licht {

const char* platform_get_current_directory();

int32 platform_create_directory(const char* directory_path);

void platform_get_directory(const char *filepath, char *dir_out, size_t size);

}