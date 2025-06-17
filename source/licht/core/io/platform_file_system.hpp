#pragma once

#include "licht/core/io/file_system.hpp"

namespace licht {

class LICHT_CORE_API PlatformFileSystem : public FileSystem {
public:
    virtual bool file_exists(StringRef p_filepath) const override;

    virtual void make_directory(StringRef p_path) override;

    virtual void remove_file(StringRef p_filepath) override;

    virtual void rename(StringRef p_path, StringRef p_new_name) override;

    virtual void move(StringRef p_subject, StringRef p_to_path) override;

    virtual FileOpenError<SharedRef<FileHandle>> open_write(StringRef p_filepath) const override;

    virtual FileOpenError<SharedRef<FileHandle>> open_read(StringRef p_filepath) const override;

public:
    virtual ~PlatformFileSystem() override;
};

}  // namespace licht