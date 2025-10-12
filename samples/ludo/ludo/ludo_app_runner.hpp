#pragma once

#include <licht/core/defines.hpp>
#include <licht/engine/engine_app_runner.hpp>

#include "ludo_exports.hpp"

using namespace licht;

class LUDO_API LudoAppRunner : public EngineAppRunner {
public:
    int32 run() override;

    void on_run_delegate();
};
