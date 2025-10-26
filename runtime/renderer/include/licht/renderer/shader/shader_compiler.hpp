#pragma once

#include "licht/core/string/string_ref.hpp"
#include "licht/rhi/compiled_shader.hpp"
#include "licht/renderer/renderer_exports.hpp"

namespace licht {

class LICHT_RENDERER_API SPIRVShaderCompiler {
public:
    enum class Stage {
        Vertex,
        Fragment,
        Compute,
        Geometry
    };

    static bool compile_file(StringRef input_filepath, StringRef output_filepath, Stage stage);
};

}  //namespace licht