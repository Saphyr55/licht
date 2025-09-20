#pragma once

#include "licht/engine/engine_exports.hpp"
#include "licht/core/containers/array.hpp"
#include "licht/core/modules/module_manifest.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

class LICHT_ENGINE_API Engine {
public:
    static Engine& get_instance();

    void startup();
    void shutdown();

    StringRef get_project_directory();
    StringRef get_engine_directory();

    Array<const ModuleManifestInformation*>& get_ordered_module_informations();

    ModuleManifest& get_manifest();

public:
    Engine();
    ~Engine() = default;

    Engine(Engine&&) = delete;
    Engine(const Engine&) = delete;

    Engine& operator=(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;

private:
    Array<const ModuleManifestInformation*> ordered_module_informations_;
    ModuleManifest manifest_;
    StringRef project_directory_;
};

}  //namespace licht