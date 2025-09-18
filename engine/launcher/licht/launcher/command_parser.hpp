#pragma once

#include "licht/core/containers/hash_map.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/launcher/launcher_exports.hpp"

namespace licht {
    
LICHT_LAUNCHER_API HashMap<StringRef, StringRef> command_line_parse(int32 argc, const char** argv);

}
