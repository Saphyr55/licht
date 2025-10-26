#pragma once

#include "ludo_exports.hpp"

#include <licht/core/modules/module.hpp>
#include <licht/core/modules/module_registry.hpp>

using namespace licht;

class LUDO_API LudoModule : public Module {
public:
    virtual void on_load() override;

    virtual void on_startup() override;

    virtual void on_shutdown() override;

    virtual void on_unload() override;
};
