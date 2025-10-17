#pragma once

namespace licht {

enum class AttributeType {
    Position,
    Normal,
    Tangent,
    UV,
    Color,
    Other,
};

struct Attribute {
    size_t count = 0;
    size_t stride = 0;
    AttributeType type = AttributeType::Other;
};

inline const char* to_string(AttributeType type) {
    switch (type) {
        case AttributeType::Position:
            return "Position";
        case AttributeType::Normal:
            return "Normal";
        case AttributeType::Tangent:
            return "Tangent";
        case AttributeType::UV:
            return "UV";
        case AttributeType::Color:
            return "Color";
        case AttributeType::Other:
            return "Other";
        default:
            return "Unknown";
    }
}


}  //namespace licht
