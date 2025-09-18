#include "project_settings.hpp"

namespace licht {

ProjectSettings& ProjectSettings::get_instance() {
    static ProjectSettings settings;
    return settings;
}

StringRef ProjectSettings::get_name(StringRef name) {
    return settings_names[name];
}

void ProjectSettings::insert(StringRef name, StringRef value) {
    settings_names[name] = String(value);
}

}  //namespace licht