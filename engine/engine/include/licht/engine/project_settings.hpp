#pragma once

#include "licht/engine/engine_exports.hpp"
#include "licht/core/containers/hash_map.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

class LICHT_ENGINE_API ProjectSettings {
public:
    static ProjectSettings& get_instance();

    StringRef get_name(StringRef name);
    void insert(StringRef name, StringRef value);

private:
    HashMap<StringRef, String> settings_names;
};

}