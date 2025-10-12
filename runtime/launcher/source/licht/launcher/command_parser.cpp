#include "licht/launcher/command_parser.hpp"

#include "licht/core/io/platform_file_system.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {

HashMap<StringRef, StringRef> command_line_parse(int32 argc, const char** argv) {
    StringRef projectdir = PlatformFileSystem::get_current_directory();
    StringRef enginedir = PlatformFileSystem::get_current_directory();

    for (uint8 i = 1; i < argc; i++) {
        StringRef arg = argv[i];

        if (arg == "--projectdir") {
            if (i + 1 >= argc) {
                LLOG_ERROR("[main]", "--projectdir requires an argument.");
                return false;
            }
            projectdir = argv[++i];
            LLOG_INFO("[main]", vformat("Set project directory to: '%s'", projectdir.data()));
            continue;
        }

        if (arg == "--enginedir") {
            if (i + 1 >= argc) {
                LLOG_ERROR("[main]", "--enginedir requires an argument.");
                return false;
            }
            enginedir = argv[++i];
            LLOG_INFO("[main]", vformat("Set engine directory to: '%s'", enginedir.data()));
            continue;
        }
    }

    return {
        {"projectdir", projectdir},
        {"enginedir", enginedir},
    };
}

}  //namespace licht