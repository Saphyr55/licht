#include "ludo_module.hpp"

// Launch header includes the main function.
#define LICHT_MAIN
#include <licht/launcher/launcher.hpp>

int32 application_launch(int32 argc, const char** argv) {
    return ludo_application_launch(argc, argv);
}
