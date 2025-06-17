#include "licht/core/io/platform_file_system.hpp"
#include "licht/core/io/platform_file_handle.hpp"
#include "licht/core/string/string_ref.hpp"

#include <stdio.h>

namespace licht {

FileSystem& FileSystem::get_platform() {
    static PlatformFileSystem platfom_file_system;
    return platfom_file_system;
}

bool PlatformFileSystem::file_exists(StringRef p_filepath) const {
#ifdef _MSC_VER
    struct _stat buffer {};

    return _stat(p_filepath, &buffer) == 0;
#else
    stat buffer{};
    return stat(filepath, &buffer) == 0;
#endif
}

void PlatformFileSystem::make_directory(StringRef p_path) {}

void PlatformFileSystem::remove_file(StringRef p_filepath) {}

void PlatformFileSystem::rename(StringRef p_path, StringRef p_new_name) {}

void PlatformFileSystem::move(StringRef p_subject, StringRef p_to_path) {}

FileOpenError<SharedRef<FileHandle>> PlatformFileSystem::open_write(StringRef p_filepath) const {
    using FileOpenErrorType = FileOpenError<SharedRef<FileHandle>>;

    FILE* stream;
    errno_t error = fopen_s(&stream, p_filepath, "wb");

    if (!stream) {
        return FileOpenErrorType::Failure(FileSystemOpenError::UNKNOWN);
    }

    SharedRef<FileHandle> file = new_ref<PlatformFileHandle>(stream);

    return FileOpenErrorType::Success(file);
}

FileOpenError<SharedRef<FileHandle>> PlatformFileSystem::open_read(StringRef p_filepath) const {
    using FileOpenErrorType = FileOpenError<SharedRef<FileHandle>>;
    
    FILE* stream;
    errno_t error = fopen_s(&stream, p_filepath, "rb");

    if (!stream) {
        return FileOpenErrorType::Failure(FileSystemOpenError::UNKNOWN);
    }

    SharedRef<FileHandle> file = new_ref<PlatformFileHandle>(stream);

    return FileOpenErrorType::Success(file);
}

PlatformFileSystem::~PlatformFileSystem() {}

}  // namespace licht