#include "licht/scene/scene_module.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {

LICHT_REGISTER_MODULE(SceneModule, "licht.scene")

void SceneModule::on_load() {
    LLOG_INFO("[SceneModule]", "Scene module loaded.");
}

void SceneModule::on_startup() {
    LLOG_INFO("[SceneModule]", "Scene module started.");
}

void SceneModule::on_shutdown() {
    LLOG_INFO("[SceneModule]", "Scene module stopped.");
}

void SceneModule::on_unload() {
    LLOG_INFO("[SceneModule]", "Scene module unloaded.");
}


}