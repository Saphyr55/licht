#pragma once

#include <licht/core/containers/array.hpp>
#include <licht/core/containers/index_range.hpp>
#include <licht/core/containers/option.hpp>
#include <licht/core/defines.hpp>
#include <licht/core/io/file_handle.hpp>
#include <licht/core/io/file_system.hpp>
#include <licht/core/memory/memory.hpp>
#include <licht/core/memory/memory_trace.hpp>
#include <licht/core/memory/shared_ref.hpp>
#include <licht/core/string/format.hpp>
#include <licht/core/trace/trace.hpp>
#include <licht/core/platform/display.hpp>
#include <licht/core/platform/display_message_handler.hpp>
#include <licht/core/platform/platform.hpp>
#include "licht/core/modules/module.hpp"
#include "licht/core/modules/module_registry.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/core/platform/window_handle.hpp"
#include "licht/rhi_vulkan/rhi_vulkan_module.hpp"

namespace licht::demo {

class DemoModule : public Module {
public:
    static constexpr StringRef Name = "demo.licht";

    virtual void on_load() override {
    }

    virtual void on_activate() override {
    }

    virtual void on_shutdown() override {
    }

    virtual void on_unload() override {
    }

};

class DemoMessageHandler : public DisplayMessageHandler {
public:
    virtual void on_window_close(WindowHandle window) override;

    virtual void on_window_resized(WindowHandle window, uint32 width, uint32 height) override;

    virtual void on_window_minized(WindowHandle window) override;

    virtual void on_window_shown(WindowHandle window) override;

    virtual void on_mouse_wheel(float32 delta) override;

    virtual void on_key_down(Key key) override;

    inline void set_rhi_module(RHIVulkanModule* rhi_module) {
        rhi_module_ = rhi_module;
    }

private:
    RHIVulkanModule* rhi_module_;
};

}  //namespace licht::demo

