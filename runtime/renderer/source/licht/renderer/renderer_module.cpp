#include "licht/renderer/renderer_module.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {

LICHT_REGISTER_MODULE(RendererModule, "licht.renderer")

void RendererModule::on_load() {
    LLOG_INFO("[RendererModule]", "RenderContext module loaded.");
}

void RendererModule::on_startup() {
    LLOG_INFO("[RendererModule]", "RenderContext module started.");
}

void RendererModule::on_shutdown() {
    LLOG_INFO("[RendererModule]", "RenderContext module stopped.");
}

void RendererModule::on_unload() {
    LLOG_INFO("[RendererModule]", "RenderContext module unloaded.");
}


}