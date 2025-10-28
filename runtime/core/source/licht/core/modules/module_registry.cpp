#include "licht/core/modules/module_registry.hpp"
#include "licht/core/io/platform_file_system.hpp"
#include "licht/core/modules/module.hpp"
#include "licht/core/platform/dynamic_library.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {

ModuleRegistry& ModuleRegistry::get_instance() {
    static ModuleRegistry s_instance;
    return s_instance;
}

Module* ModuleRegistry::load_module(const StringRef name) {
    if (is_module_loaded(name)) {
        return get_module(name);
    }

    String filepath_lib(name.data());
    filepath_lib += DynamicLibraryLoader::extension();

    SharedRef<DynamicLibrary> library = nullptr;
    if (PlatformFileSystem::get_platform().file_exists(filepath_lib)) {
        library = DynamicLibraryLoader::load(filepath_lib.data());
    } else {
        return nullptr;
    }

    if (!pending_modules_.contains(name)) {
        LLOG_ERROR("[ModuleRegistry]", vformat("Module '%s' is not registered and cannot be loaded.", name.data()))
        return nullptr;
    }

    const ModuleInitializerFunc& module_initializer = pending_modules_.get(name);
    Module* module = module_initializer();

    if (!module) {
        return nullptr;
    }

    module->on_load();

    pending_modules_.remove(name);
    loaded_modules_.put(name, LoadedModule(name, module, library));

    return module;
}

void ModuleRegistry::unload_module(const StringRef name) {
    LoadedModule* loaded_module = loaded_modules_.get_ptr(name);
    if (!loaded_module) {
        return;
    }

    if (!loaded_module->module) {
        return;
    }

    loaded_module->module->on_unload();

    if (loaded_module->library) {
        DynamicLibraryLoader::unload(loaded_module->library);
    }

    loaded_modules_.remove(name);
}

bool ModuleRegistry::exists_module(const StringRef name) const {
    return pending_modules_.contains(name) || is_module_loaded(name);
}

bool ModuleRegistry::is_module_loaded(const StringRef name) const {
    return loaded_modules_.contains(name);
}

Module* ModuleRegistry::get_module_interface(const StringRef name) {
    const LoadedModule* loaded_module = loaded_modules_.get_ptr(name);
    if (!loaded_module) {
        return nullptr;
    }
    return loaded_module->module;
}

void ModuleRegistry::register_module(const StringRef name, const ModuleInitializerFunc& initializer) {
    if (!pending_modules_.contains(name)) {
        pending_modules_.put(name, initializer);
        LLOG_DEBUG("[ModuleRegistry]", vformat("The module '%s' has been registered.", name));
    }
}

void ModuleRegistry::unregister_module(const StringRef name) {
    pending_modules_.remove(name);
}

}  //namespace licht