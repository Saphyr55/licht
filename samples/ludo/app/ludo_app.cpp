#include "licht/core/memory/shared_ref.hpp"
#include "licht/launcher/launcher.hpp"
#include "ludo_app_runner.hpp"

int main(int argc, const char* argv[]) {
    return licht::licht_main(argc, argv, licht::new_ref<LudoAppRunner>());
}
