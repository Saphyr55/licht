#pragma once

#include "licht/core/modules/module.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_exports.hpp"

namespace licht {

class LICHT_RHI_VULKAN_API RHIVulkanModule : public Module {
public:
    virtual void on_load() override;

    virtual void on_startup() override;

    virtual void on_shutdown() override;

    virtual void on_unload() override;

public:
    void set_window_handle(WindowHandle window_handle) {
        window_handle_ = window_handle;
    }

public:
    RHIVulkanModule();

private:
    WindowHandle window_handle_;
};

LICHT_REGISTER_MODULE(RHIVulkanModule, "licht.rhi.vulkan")

}  //namespace licht    