#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/containers/array_view.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/string/string.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

class Scene;
using SceneEntity = size_t;

class SceneNode {
public:
    StringRef get_name();

    uint64 get_identifier();

    SceneNode* get_parent();

    ArrayView<SceneNode*> get_children_view();

    void add_children(SceneNode* children);

    void remove_children(SceneNode* children);

    SceneNode(uint64 identifier, StringRef name);
    SceneNode(uint64 identifier, StringRef name, SceneNode* parent);
    ~SceneNode() = default;

private:
    Scene* scene_;
    Array<SceneNode*> childrens_;
    SceneNode* parent_;
    uint64 identifier_;
    String name_;
};

}