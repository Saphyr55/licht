#pragma once

#include "licht/core/modules/module_registry.hpp"

namespace licht {

class SceneModule : public Module {
public:
    virtual void on_load() override;

    virtual void on_startup() override;

    virtual void on_shutdown() override;

    virtual void on_unload() override;
};

}  //namespace licht
