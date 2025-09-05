#pragma once

#include "licht/core/collection/array.hpp"
#include "licht/core/defines.hpp"

namespace licht {

class CompiledShader {
public:
    inline Array<uint8> get_bytes() { return code_; }

public:
    CompiledShader() = default;

    CompiledShader(const Array<uint8>& code)
        : code_(code) {}

    ~CompiledShader() = default;

private:
    Array<uint8> code_;
};

}  //namespace licht