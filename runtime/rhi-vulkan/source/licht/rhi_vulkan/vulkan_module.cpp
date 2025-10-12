#include "licht/rhi_vulkan/rhi_vulkan_module.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/display.hpp"
#include "licht/rhi/rhi_module.hpp"
#include "licht/rhi_vulkan/vulkan_device.hpp"
#include "licht/rhi_vulkan/vulkan_context.hpp"

#include <vulkan/vulkan_core.h>

namespace licht {

RHIVulkanModule::RHIVulkanModule() 
    : window_handle_(Display::InvalidWindowHandle) {
}

void RHIVulkanModule::on_load() { 
    LLOG_INFO("[RHIVulkanModule]", "Loading RHI Vulkan Module.");
}

void RHIVulkanModule::on_startup() { 
    LLOG_INFO("[RHIVulkanModule]", "Startup RHI Vulkan Module.");

    ModuleRegistry& registry = ModuleRegistry::get_instance();
    RHIModule* module = registry.get_module<RHIModule>(RHIModule::ModuleName);
    
    window_handle_ = module->get_window_handle();
    module->set_device(new_ref<VulkanDevice>());
    
    void* native_window = Display::get_default().get_native_window_handle(window_handle_);
    vulkan_context_initialize( vulkan_context_get(), native_window);
}

void RHIVulkanModule::on_shutdown() { 
    vulkan_context_destroy(vulkan_context_get());
    LLOG_INFO("[RHIVulkanModule]", "Shuting down RHI Vulkan Module...");
}

void RHIVulkanModule::on_unload() { 
    LLOG_INFO("[RHIVulkanModule]", "Unload RHI Vulkan Module.");
}

}  //namespace licht