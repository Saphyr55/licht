#pragma once

#include "core/memory/shared_ref.hpp"
#include "core/string/string_ref.hpp"

namespace licht {

class LICHT_CORE_API FileHandle {
public:
    virtual int64 tell() = 0;

    virtual bool seek(int64 position) = 0;

    virtual bool read(uint8* destination, size_t nbytes) = 0;

    virtual bool write(const uint8* source, size_t nbytes) = 0;

    virtual bool flush() = 0;

    virtual size_t size() = 0;

    virtual ~FileHandle() = default;
};

}  // namespace licht