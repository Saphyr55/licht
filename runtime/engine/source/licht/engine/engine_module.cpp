#include "licht/engine/engine_module.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {
    
LICHT_REGISTER_MODULE(EngineModule, "licht.engine");

void EngineModule::on_load() {
    LLOG_INFO("[EngineModule]", "Engine module loaded.");
}

void EngineModule::on_startup() {
    LLOG_INFO("[EngineModule]", "Engine module started.");
}

void EngineModule::on_shutdown() {
    LLOG_INFO("[EngineModule]", "Engine module stopped.");
}

void EngineModule::on_unload() {
    LLOG_INFO("[EngineModule]", "Engine module unloaded.");
}

}  //namespace licht