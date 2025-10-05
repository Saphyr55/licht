#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"

namespace licht {

class SPIRVShader {
public:
    inline Array<uint8> get_bytes() { return code_; }

public:
    SPIRVShader() = default;
    
    SPIRVShader(const Array<uint8>& code)
        : code_(code) {}

    ~SPIRVShader() = default;

private:
    Array<uint8> code_;
};

}  //namespace licht