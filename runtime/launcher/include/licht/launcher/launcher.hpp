#pragma once

#include "launcher_exports.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/engine/engine_app_runner.hpp"

namespace licht {

LICHT_LAUNCHER_API int32 licht_main(int32 argc, const char** argv, SharedRef<EngineAppRunner> runner);

}  //namespace licht
