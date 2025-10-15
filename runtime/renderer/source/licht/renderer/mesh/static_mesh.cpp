#include "licht/renderer/mesh/static_mesh.hpp"
#include "licht/renderer/mesh/attribute.hpp"

namespace licht {

void StaticMesh::append_submesh(const StaticSubMesh& submesh) {
    submeshes_.append(submesh);
}


}