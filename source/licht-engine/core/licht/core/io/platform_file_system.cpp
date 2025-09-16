#include "licht/core/io/platform_file_system.hpp"
#include "licht/core/io/platform_file_handle.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/string/string_ref.hpp"

#include <stdio.h>

namespace licht {

FileSystem& FileSystem::get_platform() {
    static PlatformFileSystem platfom_file_system;
    return platfom_file_system;
}

bool PlatformFileSystem::file_exists(StringRef filepath) const {
#ifdef _MSC_VER
    struct _stat buffer {};

    return _stat(filepath, &buffer) == 0;
#else
    stat buffer{};
    return stat(filepath, &buffer) == 0;
#endif
}

void PlatformFileSystem::make_directory(StringRef path) {}

void PlatformFileSystem::remove_file(StringRef filepath) {}

void PlatformFileSystem::rename(StringRef path, StringRef new_name) {}

void PlatformFileSystem::move(StringRef subject, StringRef to_path) {}

FileOpenError<SharedRef<FileHandle>> PlatformFileSystem::open_write(StringRef filepath) const {
    using FileOpenErrorType = FileOpenError<SharedRef<FileHandle>>;

    if (!file_exists(filepath)) {
        return FileOpenErrorType::Failure(FileSystemOpenError::FileNotExist);
    }

    FILE* stream;
    errno_t error = fopen_s(&stream, filepath, "wb");

    if (!stream) {
        return FileOpenErrorType::Failure(FileSystemOpenError::Unkown);
    }

    SharedRef<FileHandle> file = new_ref<PlatformFileHandle>(stream);

    return FileOpenErrorType::Success(file);
}

FileOpenError<SharedRef<FileHandle>> PlatformFileSystem::open_read(StringRef filepath) const {
    using FileOpenErrorType = FileOpenError<SharedRef<FileHandle>>;

    if (!file_exists(filepath)) {
        return FileOpenErrorType::Failure(FileSystemOpenError::FileNotExist);
    }

    FILE* stream;
    errno_t error = fopen_s(&stream, filepath, "rb");

    if (!stream) {
        return FileOpenErrorType::Failure(FileSystemOpenError::Unkown);
    }

    SharedRef<FileHandle> file = new_ref<PlatformFileHandle>(stream);

    return FileOpenErrorType::Success(file);
}

PlatformFileSystem::~PlatformFileSystem() {}

}  // namespace licht