#include "licht/core/io/platform_file_handle.hpp"
#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"

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
    int64 current = tell();
    fseek(stream_, 0, SEEK_END);
    int64 end = ftell(stream_);
    seek(current);
    return static_cast<size_t>(end);
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

Array<uint8> PlatformFileHandle::read_all_bytes() {
    if (!stream_) {
        return {};
    }

    // Save the current position
    int64 original_pos = ftell(stream_);
    if (original_pos == -1L) {
        return {};
    }

    // Seek to end to determine size
    if (fseek(stream_, 0, SEEK_END) != 0) {
        return {};
    }

    int64 file_size = ftell(stream_);
    if (file_size == -1L) {
        return {};
    }

    // Seek back to beginning
    if (fseek(stream_, 0, SEEK_SET) != 0) {
        return {};
    }

    // Allocate buffer
    Array<uint8> buffer;
    buffer.resize(static_cast<size_t>(file_size));
    size_t bytes_read = fread(buffer.data(), sizeof(uint8), buffer.size(), stream_);
    if (bytes_read != buffer.size()) {
        return {};  // Read error or incomplete read
    }

    // Restore original position (optional)
    fseek(stream_, original_pos, SEEK_SET);

    return buffer;
}

bool PlatformFileHandle::close() {
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
    close();
}

}  // namespace licht