#pragma once

#include "licht/core/modules/module.hpp"
#include "message_exports.hpp"

namespace licht {

class LICHT_MESSAGING_API MessagingModule : public Module {
public:
    virtual void on_load() override;

    virtual void on_startup() override;

    virtual void on_shutdown() override;

    virtual void on_unload() override;

    virtual ~MessagingModule() = default;
};

}