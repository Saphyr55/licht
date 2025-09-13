#pragma once

#include "licht/core/memory/memory.hpp"

namespace licht {

class LICHT_CORE_API Module {
public:
    virtual void on_load() = 0;

    virtual void on_activate() = 0;

    virtual void on_shutdown() = 0;

    virtual void on_unload() = 0;
};

}
