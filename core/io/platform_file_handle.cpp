#include "core/io/platform_file_handle.hpp"

namespace licht {

int64 PlatformFileHandle::tell() {
    return ftell(stream_);
}

bool PlatformFileHandle::seek(int64 position) {
    if (fseek(stream_, position, SEEK_SET)) {
        position_ = position;
        return true;
    }

    return false;
}

bool PlatformFileHandle::flush() {
    fflush(stream_);
    return true;
}

size_t PlatformFileHandle::size() {
    return 0;
}

bool PlatformFileHandle::write(const uint8* buffer, size_t nbytes) {
    if (!stream_) {
        return false;
    }

    size_t s = fwrite(buffer, sizeof(uint8), nbytes, stream_);
    if (s != nbytes) {
        return false;
    }

    return true;
}

bool PlatformFileHandle::read(uint8* destination, size_t nbytes) {
    if (!destination || nbytes == 0) {
        return false;
    }

    size_t bytes_read = fread(destination, sizeof(uint8), nbytes, stream_);
    if (bytes_read != nbytes) {
        return false;
    }

    position_ += bytes_read;

    return true;
}

bool PlatformFileHandle::Close() {
    if (stream_) {
        fclose(stream_);
        stream_ = nullptr;
        return true;
    }

    return false;
}

PlatformFileHandle::PlatformFileHandle(FILE* stream)
    : stream_(stream), position_(0) {}

PlatformFileHandle::~PlatformFileHandle() {
    Close();
}

}  // namespace licht