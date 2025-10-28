#pragma once

#include "licht/entity/entity_exports.hpp"
#include "licht/core/modules/module.hpp"

namespace licht {

class LICHT_ENTITY_API EntityModule : public Module {
public:
    virtual void on_load() override;

    virtual void on_startup() override;

    virtual void on_shutdown() override;

    virtual void on_unload() override;
};

}  //namespace licht
