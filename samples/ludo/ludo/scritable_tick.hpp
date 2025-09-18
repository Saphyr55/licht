#pragma once

#include "licht/core/defines.hpp"

namespace licht {

class ScriptableTick {
public:
    virtual void on_startup() = 0;

    virtual void on_shutdown() = 0;

    virtual void on_tick(float32 delta_time) = 0;

    virtual ~ScriptableTick() = default;
};

}