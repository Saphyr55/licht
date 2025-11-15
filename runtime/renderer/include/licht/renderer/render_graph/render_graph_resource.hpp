#pragma once

#include "licht/core/defines.hpp"
#include "licht/core/string/string_ref.hpp"
#include "render_graph_node.hpp"

namespace licht {

enum class RenderGraphResourceType {
    Buffer,
    Texture,
    Attachment,
    Reference,
};

struct RenderGraphResourceInfo {

};

struct RenderGraphResource {
    RenderGraphResourceType type;
    RenderGraphResourceInfo info;

    RenderGraphNode* producer;
    RenderGraphResource* output_handle;

    int64 ref_count;
    StringRef name;
};

}  //namespace licht