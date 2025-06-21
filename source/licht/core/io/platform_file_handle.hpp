#pragma once

#include "licht/core/io/file_handle.hpp"

namespace licht {

class LICHT_CORE_API PlatformFileHandle : public FileHandle {
public:
    bool Close();

    virtual int64 tell() override;

    virtual bool seek(int64 position) override;

    virtual bool read(uint8* destination, usize nbytes) override;

    virtual Array<uint8> read_all_bytes() override;

    virtual bool write(const uint8* source, usize nbytes) override;

    virtual bool flush() override;

    virtual usize size() override;

    explicit PlatformFileHandle(FILE* stream);

    virtual ~PlatformFileHandle() override;

private:
    FILE* stream_;
    int32 position_;
};

}  // namespace licht