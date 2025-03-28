#include "demo.hpp"

#include <core/collection/array.hpp>
#include <core/collection/index_range.hpp>
#include <core/collection/option.hpp>
#include <core/defines.hpp>
#include <core/io/file_handle.hpp>
#include <core/io/file_system.hpp>
#include <core/memory/memory.hpp>
#include <core/memory/memory_trace.hpp>
#include <core/memory/shared_ref.hpp>
#include <core/string/format.hpp>
#include <core/trace/trace.hpp>
#include <display/display.hpp>
#include <platform/platform.hpp>


using namespace licht;

static bool g_is_running = false;

class DemoMessageHandler : public DisplayMessageHandler {
public:
    virtual void on_window_close(WindowHandle window) override {
        g_is_running = false;
    }
};

int32 main(int32 argc, const char** argv) {
    platform_start();

    LLOG_INFO("demo[main]", "Startup demo module.");

    Display& display = Display::get_default();
    display.set_message_handler(new_ref<DemoMessageHandler>());
    WindowHandle window_handle = display.create_window_handle({"Demo Window", 800, 600, 100, 100});

    display.show(window_handle);

    g_is_running = true;
    while (g_is_running) {
        display.pumps_events();
    }

    return 0;
}
