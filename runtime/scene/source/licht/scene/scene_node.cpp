#include "licht/scene/scene_node.hpp"

namespace licht {

StringRef SceneNode::get_name() {
    return name_;
}

uint64 SceneNode::get_identifier() {
    return identifier_;
}

SceneNode* SceneNode::get_parent() {
    return parent_;
}

ArrayView<SceneNode*> SceneNode::get_children_view() {
    return childrens_;
}

void SceneNode::add_children(SceneNode* children) {
    childrens_.append(children);
}

void SceneNode::remove_children(SceneNode* children) {
    childrens_.remove(children);
}

SceneNode::SceneNode(uint64 identifier, StringRef name)
    : identifier_(identifier), name_(name), parent_(nullptr) {
}

SceneNode::SceneNode(uint64 identifier, StringRef name, SceneNode* parent)
    : identifier_(identifier), name_(name), parent_(parent) {
    parent->add_children(this);
}

}  //namespace licht
