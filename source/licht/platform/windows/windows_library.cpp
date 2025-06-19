#include "licht/core/string/format.hpp"
#ifdef _WIN32

#include <windows.h>

#include "licht/core/defines.hpp"
#include "licht/platform/dynamic_library.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

SharedRef<DynamicLibrary> DynamicLibraryLoader::load(StringRef p_name) {

    WString wname = unicode_of_str(p_name.data());
    HMODULE handle = ::LoadLibraryW(wname.data());

    SharedRef<DynamicLibrary> library = new_ref<DynamicLibrary>(handle, p_name);

    return library;    
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