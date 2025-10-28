#pragma once

#include "licht/core/defines.hpp"
namespace licht {

class EngineAppRunner {
public:
    virtual int32 run() {
        return 0;
    };
};

}