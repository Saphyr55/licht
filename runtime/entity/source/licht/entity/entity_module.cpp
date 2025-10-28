#include "licht/entity/entity_module.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/core/modules/module_registry.hpp"

namespace licht {

LICHT_REGISTER_MODULE(EntityModule, "licht.entity");

void EntityModule::on_load() {
    LLOG_INFO("[EntityModule]", "Entity module loaded.");
}

void EntityModule::on_startup() {
    LLOG_INFO("[SchemaModule]", "Entity module started.");
}

void EntityModule::on_shutdown() {
    LLOG_INFO("[EntityModule]", "Entity module stopped.");
}

void EntityModule::on_unload() {
    LLOG_INFO("[EntityModule]", "Entity module unloaded.");
}


}