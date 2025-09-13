#include "licht/core/modules/module_registry.hpp"
#include "licht/core/modules/module.hpp"
#include "licht/core/platform/dynamic_library.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {

ModuleRegistry& ModuleRegistry::get_instance() {
    static ModuleRegistry s_instance;
    return s_instance;
}

Module* ModuleRegistry::load_module(StringRef name) {
    if (is_module_loaded(name)) {
    }

    SharedRef<DynamicLibrary> library = DynamicLibraryLoader::load(name);

    return nullptr;
}

bool ModuleRegistry::unload_module(StringRef name) {
    return false;
}

bool ModuleRegistry::module_exists(StringRef name) const {
    return false;
}

Module* ModuleRegistry::get_module(StringRef name) const {
    return nullptr;
}

bool ModuleRegistry::is_module_loaded(StringRef name) const {
    return false;
}

void ModuleRegistry::register_module(StringRef name, ModuleInitializerRef initializer) {
    pending_modules_.append({name, initializer});
    LLOG_DEBUG("[Module]", vformat("%s was registed.", name));
}

}  //namespace licht