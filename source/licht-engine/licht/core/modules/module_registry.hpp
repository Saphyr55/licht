#pragma once

#include "licht/core/function/function_ref.hpp"
#include "licht/core/memory/memory.hpp"
#include "licht/core/modules/module.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

class LICHT_CORE_API ModuleRegistry {
public:
    using ModuleInitializerRef = FunctionRef<Module*(void)>;

    static ModuleRegistry& get_instance();

    Module* load_module(StringRef name);

    bool unload_module(StringRef name);

    bool module_exists(StringRef name) const;

    Module* get_module(StringRef name) const;

    bool is_module_loaded(StringRef name) const;

    void register_module(StringRef name, ModuleInitializerRef initializer);

private:
    struct ModulePendingInitializer {
        StringRef name;
        ModuleInitializerRef initializer;
    };

    Array<ModulePendingInitializer> pending_modules_;
};

template <typename ModuleType>
class ModuleRegistrant {
public:
    static Module* initialize() {
        return Memory::new_resource<ModuleType>();
    }

public:
    ModuleRegistrant(StringRef name) {
        ModuleRegistry::get_instance().register_module(name, ModuleRegistrant<ModuleType>::initialize);
    }
};

}  //namespace licht

#define LICHT_REGISTER_MODULE(ModuleType, ModuleName) \
    static ::licht::ModuleRegistrant<ModuleType> g_module_registrant_(ModuleName);