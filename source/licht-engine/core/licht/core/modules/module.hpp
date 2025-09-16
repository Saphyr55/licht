#pragma once

#include "licht/core/core_exports.hpp"

namespace licht {

class LICHT_CORE_API Module {
public:
    virtual void on_load() = 0;

    virtual void on_startup() = 0;

    virtual void on_shutdown() = 0;

    virtual void on_unload() = 0;

    virtual ~Module() = default;
};

}
