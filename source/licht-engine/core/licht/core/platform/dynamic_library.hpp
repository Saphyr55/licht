#pragma once

#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/core/core_exports.hpp"

namespace licht {

struct DynamicLibrary {
    void* handle = nullptr;
    StringRef name = nullptr;
};

class DynamicLibraryLoader {
public:
    LICHT_CORE_API static StringRef extension();

    LICHT_CORE_API static SharedRef<DynamicLibrary> load(StringRef name);

    LICHT_CORE_API static void unload(SharedRef<DynamicLibrary> library);

    LICHT_CORE_API static void* load_function(SharedRef<DynamicLibrary> library, StringRef function_name);
};

}