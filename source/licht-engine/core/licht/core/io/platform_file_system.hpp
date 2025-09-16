#pragma once

#include "licht/core/io/file_system.hpp"

namespace licht {

class LICHT_CORE_API PlatformFileSystem : public FileSystem {
public:
    static StringRef executable_path();

    virtual bool file_exists(StringRef filepath) const override;

    virtual void make_directory(StringRef path) override;

    virtual void remove_file(StringRef filepath) override;

    virtual void rename(StringRef path, StringRef new_name) override;

    virtual void move(StringRef subject, StringRef to_path) override;

    virtual FileOpenError<SharedRef<FileHandle>> open_write(StringRef filepath) const override;

    virtual FileOpenError<SharedRef<FileHandle>> open_read(StringRef filepath) const override;

public:
    virtual ~PlatformFileSystem() override;
};

}  // namespace licht