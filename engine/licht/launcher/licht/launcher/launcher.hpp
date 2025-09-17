#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/io/platform_file_system.hpp"
#include "licht/core/modules/module_manifest.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/platform.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/launcher/launcher_exports.hpp"

namespace licht {

inline ModuleManifest g_manifest;
inline Array<const ModuleManifestInformation*> g_manifest_informations_order;

inline bool main_load_manifest(const StringRef projectdir, 
                               const StringRef engine_manifest_directory,
                               const Array<StringRef> external_deps) {

    ModuleManifest app_manifest;
    String app_manifest_filepath = vformat("%s/manifest.lua", projectdir.data());
    String engine_manifest_filepath = vformat("%s/manifest.lua", engine_manifest_directory.data());

    if (!app_manifest.load_lua(app_manifest_filepath)) {
        LLOG_ERROR("[ModuleManifest]", vformat("Cannot load the manifest '%s'", app_manifest_filepath));
        return false;
    }

    if (!g_manifest.load_lua(engine_manifest_filepath)) {
        LLOG_ERROR("[ModuleManifest]", vformat("Cannot load the manifest '%s'", engine_manifest_filepath))
        return false;
    }

    g_manifest.merge(app_manifest);

    module_manifest_log(g_manifest);

    if (!module_manifest_resolve_dependencies(g_manifest, g_manifest_informations_order)) {
        LLOG_ERROR("[ModuleManifest]", "Cannot resolve manifest dependencies.");
        return false;
    }

    for (const ModuleManifestInformation* info : g_manifest_informations_order) {
        if (!ModuleRegistry::get_instance().load_module(info->name)) {
            LLOG_ERROR("[Module]", vformat("Cannot load module: %s", info->name.data()));
            return false;
        }
    }

    return true;
}

inline void main_unload_manifest() {
    for (int32 i = g_manifest_informations_order.size() - 1; i >= 0; i--) {
        const ModuleManifestInformation* info = g_manifest_informations_order[i];
        ModuleRegistry::get_instance().unload_module(info->name);
        LLOG_INFO("[Module]", vformat("Unloaded module: %s", info->name.data()));
    }
}

inline bool main_preinit(int32 argc, const char** argv) {

    LLOG_INFO("[main]", "Launch application.");
    
    StringRef projectdir = PlatformFileSystem::get_current_directory();
    StringRef enginedir = PlatformFileSystem::get_current_directory();
    Array<StringRef> deps;

    for (uint8 i = 1; i < argc; i++) {
        StringRef arg = argv[i];
        
        if (arg == "--projectdir") {
            if (i + 1 >= argc) {
                LLOG_ERROR("[main]", "--projectdir requires an argument.");
                return false;
            }
            projectdir = argv[++i];
            LLOG_INFO("[main]", vformat("Set project directory to: '%s'", projectdir.data()));
            continue;
        }

        if (arg == "--enginedir") {
            if (i + 1 >= argc) {
                LLOG_ERROR("[main]", "--enginedir requires an argument.");
                return false;
            }
            enginedir = argv[++i];
            LLOG_INFO("[main]", vformat("Set engine directory to: '%s'", enginedir.data()));
            continue;
        }

    }

    if (!main_load_manifest(projectdir, enginedir, deps)) {
        return false;
    }

    return true;
}

inline void main_postlaunch() {
    main_unload_manifest();
}

}

#ifdef LICHT_MAIN

extern int32 application_launch(int32 argc, const char** argv);

int32 main(int32 argc, const char** argv) {
    platform_start();

    if (!licht::main_preinit(argc, argv)) {
        return EXIT_FAILURE;
    }

    const int32 result = application_launch(argc, argv);

    licht::main_postlaunch();

    platform_end();
    return result;
}

#endif
