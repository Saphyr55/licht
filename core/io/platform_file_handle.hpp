#pragma once

#include "core/io/file_handle.hpp"

namespace licht {

class LICHT_CORE_API PlatformFileHandle : public FileHandle {
public:
    bool Close();

    virtual int64 tell() override;

    virtual bool seek(int64 position) override;

    virtual bool read(uint8* destination, size_t nbytes) override;

    virtual bool write(const uint8* source, size_t nbytes) override;

    virtual bool flush() override;

    virtual size_t size() override;

    explicit PlatformFileHandle(FILE* stream);

    virtual ~PlatformFileHandle() override;

private:
    FILE* stream_;
    int32 position_;
};

}  // namespace licht