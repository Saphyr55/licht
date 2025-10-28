#include "ludo_module.hpp"

#include <licht/core/trace/trace.hpp>

LICHT_REGISTER_MODULE(LudoModule, "ludo");

void LudoModule::on_load() {
    LLOG_INFO("[LudoModule]", "Ludo module loaded.");
}

void LudoModule::on_startup() {
    LLOG_INFO("[LudoModule]", "Starting Ludo application.");
}

void LudoModule::on_shutdown() {
    LLOG_INFO("[LudoModule]", "Shutting down Ludo application.");
}

void LudoModule::on_unload() {
    LLOG_INFO("[LudoModule]", "Ludo module unloaded.");
}
