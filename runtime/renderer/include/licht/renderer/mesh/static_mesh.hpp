#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/defines.hpp"
#include "licht/renderer/material/material.hpp"
#include "licht/renderer/renderer_exports.hpp"

namespace licht {

class RHIBuffer;

struct StaticSubMesh {
    using Buffer = Array<uint8>;
    Material material;
    Buffer positions;
    Buffer normals;
    Buffer uv_textures;
    Buffer tangents;
    Array<uint32> indices;
};

class LICHT_RENDERER_API StaticMesh {
public:
    void append_submesh(const StaticSubMesh& submesh);

    const Array<StaticSubMesh>& get_submeshes() const { return submeshes_; }

    Array<StaticSubMesh>& get_submeshes() { return submeshes_; }

    StaticMesh() = default;

private:
    Array<StaticSubMesh> submeshes_;
};

}  //namespace licht