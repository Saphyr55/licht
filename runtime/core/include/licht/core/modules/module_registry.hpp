#pragma once

#include "licht/core/containers/hash_map.hpp"
#include "licht/core/function/function.hpp"
#include "licht/core/memory/default_allocator.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/modules/module.hpp"
#include "licht/core/platform/dynamic_library.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

class LICHT_CORE_API ModuleRegistry {
public:
    using ModuleInitializerFunc = Function<Module*()>;

    static ModuleRegistry& get_instance();

    Module* load_module(StringRef name);

    void unload_module(StringRef name);

    bool exists_module(StringRef name) const;

    bool is_module_loaded(StringRef name) const;

    Module* get_module_interface(StringRef name);

    template <typename ModuleType = Module>
    ModuleType* get_module(const StringRef name) {
        return static_cast<ModuleType*>(get_module_interface(name));
    }

    void register_module(StringRef name, const ModuleInitializerFunc& initializer);

    void unregister_module(StringRef name);

private:
    struct LoadedModule {
        StringRef name = "";
        Module* module = nullptr;
        SharedRef<DynamicLibrary> library;

        LoadedModule(const StringRef name, Module* module, const SharedRef<DynamicLibrary> library)
            : name(name), module(module), library(library) {}
    };

    HashMap<StringRef, LoadedModule> loaded_modules_;
    HashMap<StringRef, ModuleInitializerFunc> pending_modules_;
};

template <typename ModuleType>
class ModuleRegistrant {
public:
    ModuleRegistrant(StringRef unique_name) {
        ModuleRegistry::get_instance().register_module(unique_name, []() -> Module* {
            return lnew(DefaultAllocator::get_instance(), ModuleType());
        });
    }
};
}  //namespace licht

#define LICHT_REGISTER_MODULE_WITH_CUSTOM_VARIABLE(ModuleType, ModuleName, VariableName) \
    static const auto(VariableName) = []() {                                             \
        return ::licht::ModuleRegistrant<ModuleType>(ModuleName);                        \
    }()

// Use __COUNTER__ for uniqueness (supported by GCC, Clang, MSVC)
#define LICHT_REGISTER_MODULE(ModuleType, ModuleName) \
    LICHT_REGISTER_MODULE_WITH_CUSTOM_VARIABLE(ModuleType, ModuleName, LCONCAT(g_module_registrant_, __COUNTER__))

#if !defined(__COUNTER__)
#define LICHT_REGISTER_MODULE(ModuleType, ModuleName) \
    LICHT_REGISTER_MODULE_WITH_CUSTOM_VARIABLE(ModuleType, ModuleName, LCONCAT(g_module_registrant_, __LINE__))
#endif
