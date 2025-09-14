#include "licht/core/string/format.hpp"

#ifdef _WIN32

#include <windows.h>

#include "licht/core/defines.hpp"
#include "licht/core/platform/dynamic_library.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

StringRef DynamicLibraryLoader::extension() {
    return ".dll";
}

SharedRef<DynamicLibrary> DynamicLibraryLoader::load(StringRef name) {

    WString wname = unicode_of_str(name.data());
    HMODULE handle = ::LoadLibraryW(wname.data());

    if (!handle) {
        return SharedRef<DynamicLibrary>(nullptr);
    }

    return new_ref<DynamicLibrary>(handle, name);
}

void DynamicLibraryLoader::unload(SharedRef<DynamicLibrary> p_library) {
    
    HMODULE handle = static_cast<HMODULE>(p_library->handle);
    LCHECK(handle != nullptr);

    ::FreeLibrary(handle);
}

void* DynamicLibraryLoader::load_function(SharedRef<DynamicLibrary> p_library, StringRef p_function_name) {

    LCHECK(p_library->handle != nullptr);

    HMODULE handle = static_cast<HMODULE>(p_library->handle);
    FARPROC function = ::GetProcAddress(handle, p_function_name);

    if (function == nullptr) {
        return nullptr;
    }

    return reinterpret_cast<void*>(function);
}

}

#endif // WIN32