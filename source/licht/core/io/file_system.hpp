#pragma once

#include "licht/core/collection/result.hpp"
#include "licht/core/core_exports.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/memory/shared_ref.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

class FileHandle;

enum class FileSystemOpenError : uint8 {
    UNKNOWN
};

template <typename T>
using FileOpenError = Result<T, FileSystemOpenError>;

class LICHT_CORE_API FileSystem {
public:
    static FileSystem& get_platform();

    virtual bool file_exists(StringRef filepath) const = 0;

    virtual void make_directory(StringRef path) = 0;

    virtual void remove_file(StringRef filepath) = 0;

    virtual void rename(StringRef path, StringRef new_name) = 0;

    virtual void move(StringRef subject, StringRef toPath) = 0;

    virtual FileOpenError<SharedRef<FileHandle>> open_write(StringRef filepath) const = 0;

    virtual FileOpenError<SharedRef<FileHandle>> open_read(StringRef filepath) const = 0;

public:
    virtual ~FileSystem() = default;
};

}  // namespace licht