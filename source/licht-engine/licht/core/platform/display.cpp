#include "licht/core/defines.hpp"
#include "licht/core/platform/display.hpp"

namespace licht {

Display* DisplayProvider::display_ = nullptr;

Display* DisplayProvider::get_display() {
    return display_;
}

void DisplayProvider::set_display(Display* display) {
    display_ = display;
}

Display& Display::get_default() {
    Display* display = DisplayProvider::get_display();
    LCHECK_MSG(display != nullptr, "Display is not initialized.");
    return *display;
}

}  //namespace licht