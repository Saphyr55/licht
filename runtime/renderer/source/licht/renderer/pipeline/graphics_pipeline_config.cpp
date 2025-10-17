#include "licht/renderer/pipeline/graphics_pipeline_config.hpp"
#include "licht/core/io/file_handle.hpp"
#include "licht/core/io/file_system.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

GraphicsPipelineConfigBuilder& GraphicsPipelineConfigBuilder::with_vertex_shader_from_file(StringRef path, StringRef entry) {
    FileHandleResult fh = FileSystem::get_platform().open_read(path);
    LCHECK(fh.has_value());
    SharedRef<FileHandle> file = fh.value();
    SPIRVShader shader(file->read_all_bytes());
    cfg_.vertex_shader = shader;
    cfg_.vertex_entry = std::move(entry);
    return *this;
}

GraphicsPipelineConfigBuilder& GraphicsPipelineConfigBuilder::with_fragment_shader_from_file(StringRef path, StringRef entry) {
    FileHandleResult fh = FileSystem::get_platform().open_read(path);
    LCHECK(fh.has_value());
    SharedRef<FileHandle> file = fh.value();
    SPIRVShader shader(file->read_all_bytes());
    cfg_.fragment_shader = shader;
    cfg_.fragment_entry = std::move(entry);
    return *this;
}

}  //namespace licht
