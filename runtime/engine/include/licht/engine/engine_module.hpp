#pragma once

#include "licht/core/modules/module.hpp"
#include "licht/engine/engine_exports.hpp"

namespace licht {

class LICHT_ENGINE_API EngineModule : public Module {
public:
    virtual void on_load() override;

    virtual void on_startup() override;

    virtual void on_shutdown() override;

    virtual void on_unload() override;

    virtual ~EngineModule() = default;
};

}