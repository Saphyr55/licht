#include "licht/launcher/launcher_module.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {
    
LICHT_REGISTER_MODULE(LauncherModule, "licht.launcher");

void LauncherModule::on_load() {
    LLOG_INFO("[LauncherModule]", "Launcher module loaded.");
}

void LauncherModule::on_startup() {
    LLOG_INFO("[LauncherModule]", "Launcher module started.");
}

void LauncherModule::on_shutdown() {
    LLOG_INFO("[LauncherModule]", "Launcher module stopped.");
}

void LauncherModule::on_unload() {
    LLOG_INFO("[LauncherModule]", "Launcher module unloaded.");
}

}  //namespace licht