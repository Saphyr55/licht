#include "licht/rhi/rhi_module.hpp"
#include "licht/core/platform/display.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {

StringRef graphics_api_module_name(const GraphicsAPI graphics_api) {
    switch (graphics_api) {
        case GraphicsAPI::Vulkan: {
            return "licht.rhi.vulkan";
        }
    }
    return "licht.rhi.vulkan";
}

RHIModule::RHIModule()
    : window_handle_(Display::InvalidWindowHandle)
    , graphics_api_(GraphicsAPI::Vulkan) {}

void RHIModule::on_load() {
    LLOG_INFO("[RHIModule]", "Loading RHI Module.");
}

void RHIModule::on_startup() {
    LLOG_INFO("[RHIModule]", "Start up RHI Module.");

    ModuleRegistry& module_registry = ModuleRegistry::get_instance();
    Module* module = module_registry.get_module_interface(graphics_api_module_name(graphics_api_));
    module->on_startup();
}

void RHIModule::on_shutdown() {
    LLOG_INFO("[RHIModule]", "Shuting down RHI Module...");
    
    ModuleRegistry& module_registry = ModuleRegistry::get_instance();
    Module* module = module_registry.get_module_interface(graphics_api_module_name(graphics_api_));
    module->on_shutdown();
}

void RHIModule::on_unload() {
    LLOG_INFO("[RHIModule]", "Unload RHI Module.");
}

}  //namespace licht