#pragma once

#include "licht/core/containers/array.hpp"
#include "licht/core/containers/array_view.hpp"
#include "licht/core/defines.hpp"
#include "licht/core/math/math.hpp"
#include "licht/core/string/string_ref.hpp"
#include "licht/core/trace/trace.hpp"
#include "licht/renderer/mesh/attribute.hpp"
#include "licht/renderer/mesh/static_submesh.hpp"
#include "licht/renderer/renderer_exports.hpp"
#include "licht/core/math/vector3.hpp"
#include "licht/core/math/vector4.hpp"

namespace licht {

class RHIBuffer;

struct StaticSubMesh {
    using Buffer = Array<uint8>;
    Material material;
    Buffer positions;
    Buffer uv_textures;
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