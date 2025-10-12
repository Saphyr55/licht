#pragma once

#include "licht/core/modules/module.hpp"
#include "message_exports.hpp"

namespace licht {

class LICHT_MESSAGING_API MessageModule : public Module {
public:
    virtual void on_load() = 0;

    virtual void on_startup() = 0;

    virtual void on_shutdown() = 0;

    virtual void on_unload() = 0;

    virtual ~MessageModule() = default;
};

}