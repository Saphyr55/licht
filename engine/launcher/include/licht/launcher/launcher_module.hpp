#pragma once

#include "licht/core/modules/module.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/launcher/launcher_exports.hpp"

namespace licht {

class LICHT_LAUNCHER_API LauncherModule : public Module {
public:
    static constexpr const char* ModuleName = "licht.launcher";

    virtual void on_load() override;

    virtual void on_startup() override;

    virtual void on_shutdown() override;

    virtual void on_unload() override;

    virtual ~LauncherModule() = default;
};

LICHT_REGISTER_MODULE(LauncherModule, LauncherModule::ModuleName)

}