#include "licht/renderer/mesh/static_mesh.hpp"

namespace licht {

void StaticMesh::append_submesh(const StaticSubMesh& submesh) {
    submeshes_.append(submesh);
}

}