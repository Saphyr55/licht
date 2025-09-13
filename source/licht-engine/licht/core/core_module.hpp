#pragma once

#include "licht/core/modules/module_registry.hpp"

namespace licht {

class CoreModule : public Module {
    virtual void on_load() {}

    virtual void on_activate() {}

    virtual void on_shutdown() {}

    virtual void on_unload() {}
};

}  //namespace licht

LICHT_REGISTER_MODULE(::licht::CoreModule, "licht.core")