#include "demo.hpp"
#include "app_message_handler.hpp"

#include <licht/core/modules/module_manifest.hpp>
#include <licht/core/string/format.hpp>
#include <licht/core/trace/trace.hpp>
#include <licht/core/defines.hpp>
#include <licht/core/memory/shared_ref.hpp>
#include <licht/core/platform/window_handle.hpp>
#include <licht/rhi_vulkan/rhi_vulkan_module.hpp>

namespace licht::demo {

int32 launch(int32 argc, const char** argv) {

    ModuleManifest manifest;
    if (!manifest.load_from_lua_manifest("../../../manifest.lua")) {
        return EXIT_FAILURE;
    }
    module_manifest_log(manifest);

    Array<const ModuleManifestInformation *> order;
    module_manifest_dependencies_resolve(manifest, order);

    for (const ModuleManifestInformation* info : order) {
        LLOG_INFO("[ModuleManifest][main]", vformat("%s", info->name.data()))
    }
    
    SharedRef<DemoMessageHandler> demo_message_handler = new_ref<DemoMessageHandler>();
    Display::get_default().set_message_handler(demo_message_handler);

    WindowStatues window_statues("Demo Window", 800, 600, 100, 100);
    WindowHandle window_handle = Display::get_default().create_window_handle(window_statues);

    RHIVulkanModule rhi_module;
    rhi_module.set_window_handle(window_handle);
    
    rhi_module.initialize();

    demo_message_handler->set_rhi_module(&rhi_module);

    Display::get_default().show(window_handle);

    g_is_running = true;
    while (g_is_running) {
        Display::get_default().handle_events();
        rhi_module.tick();
    }

    rhi_module.shutdown();

    return EXIT_SUCCESS;
}

}

int32 main(int32 p_argc, const char** p_argv) {

    LLOG_INFO("[main]", "Launch application.");
    ::licht::platform_start();

    int32 result = licht::demo::launch(p_argc, p_argv);

    ::licht::platform_end();
    
    return result;
}

