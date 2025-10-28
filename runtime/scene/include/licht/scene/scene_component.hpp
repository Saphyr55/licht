#pragma once

#include "licht/core/math/matrix4.hpp"
#include "licht/core/string/string.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/entity/entity.hpp"
#include "licht/entity/registry.hpp"

namespace licht {

struct NameComponent {
    String name;

    NameComponent() = default;
    NameComponent(StringRef in_name)
        : name(in_name) {}
};

struct TransformComponent {
    Matrix4f local;
    Matrix4f world;
    bool dirty{true};
    
    TransformComponent() = default;
    TransformComponent(const Matrix4f& in_local) : local(in_local) {}
};

struct HierarchyComponent {
    Entity parent = EntityRegistry::invalid_entity;
    Array<Entity> children;

    bool is_root() const {
        return parent == EntityRegistry::invalid_entity;
    }

    bool has_children() const {
        return !children.empty();
    }
};

}  //namespace licht