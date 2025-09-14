#include "demo.hpp"
#include "app_message_handler.hpp"

#include <licht/core/modules/module_registry.hpp>
#include <licht/core/platform/display.hpp>
#include <licht/core/platform/platform.hpp>
#include <licht/rhi/rhi_module.hpp>
#include <licht/core/modules/module_manifest.hpp>
#include <licht/core/string/format.hpp>
#include <licht/core/trace/trace.hpp>
#include <licht/core/defines.hpp>
#include <licht/core/memory/shared_ref.hpp>
#include <licht/core/platform/window_handle.hpp>

namespace licht::demo {

static int32 launch(int32 argc, const char** argv) {

    ModuleManifest manifest;
    if (!manifest.load_lua("../../../manifest.lua")) {
        return EXIT_FAILURE;
    }
    module_manifest_log(manifest);

    Array<const ModuleManifestInformation*> order;
    if (!module_manifest_dependencies_resolve(manifest, order)) {
        LLOG_ERROR("[ModuleManifest][main]", "Dependencies cycle.");
        return EXIT_FAILURE;
    }

    ModuleRegistry& module_registry = ModuleRegistry::get_instance();        
    for (const ModuleManifestInformation* info : order) {
        if (!module_registry.load_module(info->name)) {
            return EXIT_FAILURE;
        }
    }

    SharedRef<DemoMessageHandler> demo_message_handler = new_ref<DemoMessageHandler>();
    Display::get_default().set_message_handler(demo_message_handler);

    const WindowStatues window_statues("Demo Window", 800, 600, 100, 100);
    const WindowHandle window_handle = Display::get_default().create_window_handle(window_statues);
    
    RHIModule* rhi_module = module_registry.get_module<RHIModule>("licht.engine.rhi");
    rhi_module->set_window_handle(window_handle);
    rhi_module->on_startup();

    demo_message_handler->set_rhi_module(rhi_module);

    Display::get_default().show(window_handle);

    g_is_running = true;
    while (g_is_running) {
        Display::get_default().handle_events();
        rhi_module->on_tick();
    }

    rhi_module->on_shutdown();

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

