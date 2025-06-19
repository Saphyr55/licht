#include "rhi_vulkan_module.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/platform/display.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_loader.hpp"
#include "licht/rhi_vulkan/wrapper/vulkan_context.hpp"

namespace licht {

void RHIVulkanModule::initialize() {
    LLOG_INFO("[RHIVulkanModule::initialize]", "Initializing Vulkan RHI module...");

    LLOG_FATAL_WHEN(!Display::get_default().is_valid(Display::MAIN_WINDOW_HANDLE), "[RHIVulkanModule::initialize]",
                    "Failed to retrieve a valid window handle. Ensure a window is created before initializing the Vulkan RHI module.");

    void* window_handle = Display::get_default().get_native_window_handle(Display::MAIN_WINDOW_HANDLE);

    context_ = vulkan_context_create(window_handle);
    LLOG_FATAL_WHEN(!context_, "[RHIVulkanModule::initialize]", "Failed to create Vulkan RHI context.");
}

void RHIVulkanModule::shutdown() {
    vulkan_context_destroy(context_);

    LLOG_INFO("[RHIVulkanModule::shutdown]", "Shutting down Vulkan RHI module...");
}

}  //namespace licht