#pragma once

#include "licht/core/modules/module.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/rhi/rhi_exports.hpp"

namespace licht {

class LICHT_RHI_API RHIModule : public Module {
public:
    virtual void on_load() override;

    virtual void on_activate() override;

    virtual void on_shutdown() override;

    virtual void on_unload() override;
};

LICHT_REGISTER_MODULE(RHIModule, "licht.engine.rhi")

}  //namespace licht