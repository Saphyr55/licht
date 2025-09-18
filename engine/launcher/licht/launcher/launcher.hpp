#pragma once

#include "command_parser.hpp"
#include "licht/core/containers/hash_map.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/io/platform_file_system.hpp"
#include "licht/core/modules/module_manifest.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/platform.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/engine/engine.hpp"
#include "licht/engine/project_settings.hpp"
#include "licht/launcher/launcher_exports.hpp"

namespace licht {

inline bool main_load_manifest() {
    
    Engine& engine = Engine::get_instance();
    ModuleManifest& engine_manifest = engine.get_manifest();
    
    StringRef engine_manifest_directory = engine.get_engine_directory();
    StringRef projectdir = engine.get_project_directory(); 

    ModuleManifest app_manifest;
    String app_manifest_filepath = vformat("%s/manifest.lua", projectdir.data());
    String engine_manifest_filepath = vformat("%s/manifest.lua", engine_manifest_directory.data());

    if (!app_manifest.load_lua(app_manifest_filepath)) {
        LLOG_ERROR("[ModuleManifest]", vformat("Cannot load the manifest '%s'", app_manifest_filepath.data()));
        return false;
    }

    if (!engine.get_manifest().load_lua(engine_manifest_filepath)) {
        LLOG_ERROR("[ModuleManifest]", vformat("Cannot load the manifest '%s'", engine_manifest_filepath.data()))
        return false;
    }

    engine_manifest.merge(app_manifest);

    module_manifest_log(engine_manifest);
    
    Array<const ModuleManifestInformation*>& order = engine.get_ordered_module_informations();

    if (!module_manifest_resolve_dependencies(engine_manifest, order)) {
        LLOG_ERROR("[ModuleManifest]", "Cannot resolve manifest dependencies.");
        return false;
    }

    for (const ModuleManifestInformation* info : engine.get_ordered_module_informations()) {
        if (!ModuleRegistry::get_instance().load_module(info->name)) {
            LLOG_ERROR("[Module]", vformat("Cannot load module: %s", info->name.data()));
            return false;
        }
    }

    return true;
}

inline void main_unload_manifest() {
    Engine& engine = Engine::get_instance();
    for (int32 i = engine.get_ordered_module_informations().size() - 1; i >= 0; i--) {
        const ModuleManifestInformation* info = engine.get_ordered_module_informations()[i];
        ModuleRegistry::get_instance().unload_module(info->name);
        LLOG_INFO("[Module]", vformat("Unloaded module: %s", info->name.data()));
    }
}

inline bool main_preinit(int32 argc, const char** argv) {
    LLOG_INFO("[main]", "Pre init engine.");

    Engine& engine = Engine::get_instance();

    HashMap<StringRef, StringRef> commands = command_line_parse(argc, argv);
    StringRef projectdir = commands["projectdir"];
    StringRef enginedir = commands["enginedir"];

    engine.startup();

    ProjectSettings& settings = ProjectSettings::get_instance();
    settings.insert("projectdir", projectdir);
    settings.insert("enginedir", enginedir);

    if (!main_load_manifest()) {
        return false;
    }

    return true;
}

inline void main_postlaunch() {
    Engine& engine = Engine::get_instance();

    main_unload_manifest();
    engine.shutdown();
}

}  //namespace licht

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
