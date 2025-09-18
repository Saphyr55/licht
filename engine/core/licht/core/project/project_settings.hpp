#pragma once

#include "licht/core/string/string_ref.hpp"

namespace licht {

class ProjectSettings {
public:
    StringRef get_project_directory();
    StringRef get_engine_directory();

public:
    ProjectSettings();

private:
    StringRef project_directory_;
    StringRef engine_directory_;
};

}