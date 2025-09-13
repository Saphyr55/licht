#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

struct ModuleManifestInformation {
    String name = "";
    String version = "";
    Array<String> dependencies = {};
};

class LICHT_CORE_API ModuleManifestKeyNames {
public:
    static constexpr StringRef Name = "name";
    static constexpr StringRef Version = "version";
    static constexpr StringRef Dependencies = "dependencies";
};

class LICHT_CORE_API ModuleManifest {
public:
    bool load_from_lua_manifest(StringRef filepath);

    Array<ModuleManifestInformation>& get_manifest_informations();
    const Array<ModuleManifestInformation>& get_manifest_informations() const;

private:
    Array<ModuleManifestInformation> manifest_informations_;
};

LICHT_CORE_API void log_module_manifest(const ModuleManifest& manifest);

}