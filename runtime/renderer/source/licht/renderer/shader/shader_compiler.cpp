#include "licht/core/string/string.hpp"
#include "licht/renderer/shader/shader_compiler.hpp"
#include "licht/core/trace/trace.hpp"

namespace licht {

bool SPIRVShaderCompiler::compile_file(StringRef input_filepath, StringRef output_filepath, Stage stage) {

    String stage_flag;
    switch (stage) {
        case Stage::Vertex:
            stage_flag = "-V -S vert";
            break;
        case Stage::Fragment:
            stage_flag = "-V -S frag";
            break;
        case Stage::Compute:
            stage_flag = "-V -S comp";
            break;
        case Stage::Geometry:
            stage_flag = "-V -S geom";
            break;
    }

    String cmd = "glslangValidator ";
    cmd.append(stage_flag);
    cmd.append(" ");
    cmd.append(input_filepath);
    cmd.append(" -o ");
    cmd.append(output_filepath);
    cmd.append(" -Os");

    int32 result = std::system(cmd.data());
    
    if (result != 0) {
        LLOG_ERROR("[SPIRVShaderCompiler]", vformat("Failed to compile shader file: %s", input_filepath.data()));
        return false;
    }

    LLOG_INFO("[SPIRVShaderCompiler]", vformat("Successfully compiled shader file: %s", input_filepath.data()));
    LLOG_INFO("[SPIRVShaderCompiler]", vformat("Output spv file: %s", output_filepath.data()));    

    return true;
}

}  //namespace licht