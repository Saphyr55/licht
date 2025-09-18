#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

struct ModuleManifestInformation {
    Array<String> dependencies = {};
    String name = "";
    String version = "";
};

class LICHT_CORE_API ManifestKeyNames {
public:
    static constexpr StringRef Modules = "modules";
};

class LICHT_CORE_API ModuleManifestKeyNames {
public:
    static constexpr StringRef Name = "name";
    static constexpr StringRef Version = "version";
    static constexpr StringRef Dependencies = "dependencies";
};

class LICHT_CORE_API ModuleManifest {
public:
    bool load_lua(StringRef filepath);

    bool merge(const ModuleManifest& other);

public:
    Array<ModuleManifestInformation>& get_manifest_informations();
    const Array<ModuleManifestInformation>& get_manifest_informations() const;

private:
    Array<ModuleManifestInformation> manifest_informations_;
};

LICHT_CORE_API bool module_manifest_resolve_dependencies(const ModuleManifest& manifest, Array<const ModuleManifestInformation*>& out_order);
LICHT_CORE_API void module_manifest_log(const ModuleManifest& manifest);

}