#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/containers/array_view.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/math/vector3.hpp"
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
    Array<uint32> indices;

    ArrayView<Vector3f> view_positions() {
        return ArrayView<Vector3f>(reinterpret_cast<Vector3f*>(positions.data()), positions.size() / sizeof(Vector3f)); 
    }

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