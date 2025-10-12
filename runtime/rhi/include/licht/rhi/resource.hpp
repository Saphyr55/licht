#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

class RHIResource {
public:
    virtual ~RHIResource() = default;
};

template <typename T>
class RHIResourceHandle {
public:
    static constexpr const uint64 s_invalid_identifier = UINT64_MAX;

public:
    RHIResourceHandle()
        : name_()
        , identifier_(s_invalid_identifier) {}

    RHIResourceHandle(StringRef name, uint64 identifier)
        : name_(name)
        , identifier_(identifier) {}

    StringRef ge_name() { return name_; }

    bool is_valid() const { return identifier_ != 0; }

    uint64 get_identifier() const { return identifier_; }

private:
    StringRef name_;
    uint64 identifier_;
};

}  //namespace licht
