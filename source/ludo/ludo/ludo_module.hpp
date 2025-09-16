#pragma once

#include "ludo_exports.hpp"

#include <licht/core/modules/module.hpp>
#include <licht/core/modules/module_registry.hpp>

using namespace licht;

LICHT_LUDO_API int32 ludo_application_launch(int32 argc, const char** argv);

class LICHT_LUDO_API LudoModule : public Module {
public:
    virtual void on_load() override {
    }

    virtual void on_startup() override {
    }

    virtual void on_shutdown() override {
    }

    virtual void on_unload() override {
    }
};

LICHT_REGISTER_MODULE(LudoModule, "ludo")
