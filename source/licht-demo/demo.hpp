#pragma once

#include <licht/core/collection/array.hpp>
#include <licht/core/collection/index_range.hpp>
#include <licht/core/collection/option.hpp>
#include <licht/core/defines.hpp>
#include <licht/core/io/file_handle.hpp>
#include <licht/core/io/file_system.hpp>
#include <licht/core/memory/memory.hpp>
#include <licht/core/memory/memory_trace.hpp>
#include <licht/core/memory/shared_ref.hpp>
#include <licht/core/string/format.hpp>
#include <licht/core/trace/trace.hpp>
#include <licht/platform/display.hpp>
#include <licht/platform/platform.hpp>
#include <licht/platform/display_message_handler.hpp>
#include "licht/rhi_vulkan/rhi_vulkan_module.hpp"

namespace licht::demo {

class DemoMessageHandler : public DisplayMessageHandler {
public:
    virtual void on_window_close(WindowHandle p_window) override;

    virtual void on_window_resized(WindowHandle p_window, uint32 p_width, uint32 p_height) override;

    virtual void on_mouse_wheel(float32 p_delta) override;

    virtual void on_key_down(Key p_key) override;

    inline void set_rhi_module(RHIVulkanModule* p_rhi_module) {
        rhi_module_ = p_rhi_module;
    }

private:
    RHIVulkanModule* rhi_module_;
};

}
