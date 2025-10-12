#include "licht/launcher/launcher.hpp"

#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/platform/platform.hpp"
#include "licht/core/containers/hash_map.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/modules/module_manifest.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/engine/engine.hpp"
#include "licht/engine/engine_app_runner.hpp"
#include "licht/engine/project_settings.hpp"
#include "licht/launcher/command_parser.hpp"

namespace licht {

bool main_load_manifest() {
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

void main_unload_manifest() {
    Engine& engine = Engine::get_instance();
    for (int32 i = engine.get_ordered_module_informations().size() - 1; i >= 0; i--) {
        const ModuleManifestInformation* info = engine.get_ordered_module_informations()[i];
        ModuleRegistry::get_instance().unload_module(info->name);
        LLOG_INFO("[Module]", vformat("Unloaded module: %s", info->name.data()));
    }
}

bool main_preinit(int32 argc, const char** argv) {
    LLOG_INFO("[main]", "Pre init engine.");

    HashMap<StringRef, StringRef> commands = command_line_parse(argc, argv);
    StringRef projectdir = commands["projectdir"];
    StringRef enginedir = commands["enginedir"];

    ProjectSettings& settings = ProjectSettings::get_instance();
    settings.insert("projectdir", projectdir);
    settings.insert("enginedir", enginedir);

    if (!main_load_manifest()) {
        return false;
    }

    return true;
}

void main_postlaunch() {
    main_unload_manifest();
}

int32 engine_run(SharedRef<EngineAppRunner> runner) {
    Engine& engine = Engine::get_instance();
    engine.startup();

    const int32 result = runner->run();

    engine.shutdown();

    return result;
}

int32 licht_main(int32 argc, const char** argv, SharedRef<EngineAppRunner> runner) {
    platform_start();
    if (!main_preinit(argc, argv)) {
        return EXIT_FAILURE;
    }

    const int32 result = engine_run(runner);

    main_postlaunch();
    platform_end();

    return result;
}

}  //namespace licht