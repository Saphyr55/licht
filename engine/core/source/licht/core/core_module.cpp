#include "licht/core/core_module.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {
    
void CoreModule::on_load() {
    LLOG_INFO("[CoreModule]", "Core module loaded.");
}

void CoreModule::on_startup() {
    LLOG_INFO("[CoreModule]", "Core module started.");
}

void CoreModule::on_shutdown() {
    LLOG_INFO("[CoreModule]", "Core module stopped.");
}

void CoreModule::on_unload() {
    LLOG_INFO("[CoreModule]", "Core module unloaded.");
}

}