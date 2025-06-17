#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/platform/platform_exports.hpp"

namespace licht {

struct DynamicLibrary {
    void* handle = nullptr;
    StringRef name = nullptr;
};

class DynamicLibraryLoader {
public:
    LICHT_PLATFORM_API static SharedRef<DynamicLibrary> load(StringRef p_name);

    LICHT_PLATFORM_API static void unload(SharedRef<DynamicLibrary> p_library);

    LICHT_PLATFORM_API static void* load_function(SharedRef<DynamicLibrary> p_library, StringRef p_function_name);
};

}