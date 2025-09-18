#include "engine.hpp"
#include "project_settings.hpp"

namespace licht {

Engine::Engine() 
    : ordered_module_informations_(10) {
}

Engine& Engine::get_instance() {
    static Engine s_engine;
    return s_engine;
}

void Engine::startup() {

}

void Engine::shutdown() {

}

StringRef Engine::get_project_directory() {
    return ProjectSettings::get_instance().get_name("projectdir");
}

StringRef Engine::get_engine_directory() {
    return ProjectSettings::get_instance().get_name("enginedir");
}

Array<const ModuleManifestInformation*>& Engine::get_ordered_module_informations() {
    return ordered_module_informations_;
}

ModuleManifest& Engine::get_manifest() {
    return manifest_;
}

}  //namespace licht