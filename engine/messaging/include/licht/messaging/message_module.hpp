#pragma once

#include "licht/core/modules/module.hpp"

namespace licht {

class MessageModule : public Module {
public:
    virtual void on_load() = 0;

    virtual void on_startup() = 0;

    virtual void on_shutdown() = 0;

    virtual void on_unload() = 0;

    virtual ~MessageModule() = default;
};

}