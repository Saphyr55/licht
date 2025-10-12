#include "licht/core/string/format.hpp"

#ifdef _WIN32

#include "licht/core/platform/windows/windows.hpp"

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

void DynamicLibraryLoader::unload(SharedRef<DynamicLibrary> library) {
    LCHECK(library);

    HMODULE handle = static_cast<HMODULE>(library->handle);
    LCHECK(handle);

    ::FreeLibrary(handle);
}

void* DynamicLibraryLoader::load_function(SharedRef<DynamicLibrary> library, StringRef function_name) {
    LCHECK(library);
    LCHECK(library->handle);

    HMODULE handle = static_cast<HMODULE>(library->handle);
    FARPROC function = ::GetProcAddress(handle, function_name);

    if (function == nullptr) {
        return nullptr;
    }

    return reinterpret_cast<void*>(function);
}

}

#endif // WIN32