#include "licht/schema/schema_module.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/core/modules/module_registry.hpp"

namespace licht {

LICHT_REGISTER_MODULE(SchemaModule, "licht.schema")

void SchemaModule::on_load() {
    LLOG_INFO("[SchemaModule]", "Schema module loaded.");
}

void SchemaModule::on_startup() {
    LLOG_INFO("[SchemaModule]", "Schema module started.");
}

void SchemaModule::on_shutdown() {
    LLOG_INFO("[SchemaModule]", "Schema module stopped.");
}

void SchemaModule::on_unload() {
    LLOG_INFO("[SchemaModule]", "Schema module unloaded.");
}


}