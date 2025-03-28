#ifdef _WIN32
#include <Windows.h>

#include "platform/windows/windows_display.hpp"
#include "platform/platform.hpp"
#include "display/display.hpp"

namespace licht {

static WindowsDisplay g_windows_display(GetModuleHandle(nullptr));

void platform_start() {
    DisplayProvider::set_display(&g_windows_display);
}

void platform_end() {
}

}  //namespace licht

#endif // _WIN32
