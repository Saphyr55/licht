#pragma once

#include <licht/core/modules/module.hpp>
#include <licht/core/modules/module_registry.hpp>

namespace licht::demo {

class DemoModule : public Module {
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

LICHT_REGISTER_MODULE(DemoModule, "licht.demo")

}  //namespace licht::demo

